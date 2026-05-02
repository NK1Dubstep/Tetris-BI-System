from fastapi import FastAPI, WebSocket
from fastapi.responses import JSONResponse
from fastapi.staticfiles import StaticFiles
import json
import asyncio
from datetime import datetime
import psycopg2
from psycopg2 import pool
from psycopg2.extras import RealDictCursor
from contextlib import contextmanager

import random

DB_CONFIG = {
  "dbname": "tetris_bi",
  "user": "postgres",
  "password": "12345",
  "host": "127.0.0.1",
  "port": "5432"
}

is_playing_number = 0
is_playing_tetris_number = 0

connection_pool = pool.ThreadedConnectionPool(1, 20, **DB_CONFIG)

@contextmanager
def get_db():
  conn = connection_pool.getconn()
  try:
    yield conn
    conn.commit()
  except Exception:
    conn.rollback()
    raise
  finally:
    connection_pool.putconn(conn)

app = FastAPI()

active_connections = []
websocket_ids = {}
is_playing_tetris_state = {}

@app.post("/tetris_bi/login_as_free_player")
def login_as_free_player():
  with get_db() as conn:
    with conn.cursor() as cur:
      cur.execute("""
        UPDATE players
        SET is_playing = TRUE, last_login = NOW()
        WHERE id = (
          SELECT id FROM players
          WHERE is_playing = FALSE
          ORDER BY RANDOM()
          LIMIT 1
          FOR UPDATE SKIP LOCKED
        )
        RETURNING id
      """)
      id = cur.fetchone()

  if not id:
    return JSONResponse(content={"message": "No free players found"}, status_code=404)

  return JSONResponse(content={"message": "free player successfully found", "id": id}, status_code=200)


@app.websocket("/ws")
async def websocket_endpoint(websocket: WebSocket):
  global is_playing_number, is_playing_tetris_number

  await websocket.accept()
  active_connections.append(websocket)

  try:
    while True:
      data = await websocket.receive_text()
      message = json.loads(data)

      if message["type"] == "register":
        with get_db() as conn:
          with conn.cursor() as cur:
            cur.execute("""
              INSERT INTO players (with_us_since, last_login, is_playing)
              VALUES (NOW(), NOW(), TRUE)
              RETURNING id
            """)
            id = cur.fetchone()[0]

        await websocket.send_text(json.dumps({
          "message": "register",
          "id": id,
          "bot_index": message["bot_index"]
        }))
        ids = websocket_ids.get(websocket)
        if ids is None:
          websocket_ids[websocket] = [id]
        else:
          websocket_ids[websocket].append(id)

      from psycopg2.extras import execute_values
      '''
      if message["type"] == "register_batch":
        number = message["number"]
        BATCH_SIZE = 1000  # оптимальный размер пачки
        all_new_ids = []

        with get_db() as conn:
          with conn.cursor() as cur:
            for batch_start in range(0, number, BATCH_SIZE):
              batch_end = min(batch_start + BATCH_SIZE, number)
              batch_number = batch_end - batch_start

              values = ','.join(['(NOW(), NOW(), TRUE)'] * batch_number)
              query = f"INSERT INTO players (with_us_since, last_login, is_playing) VALUES {values} RETURNING id"

              cur.execute(query)
              batch_ids = [row[0] for row in cur.fetchall()]
              all_new_ids.extend(batch_ids)

        # Обновляем словарь websocket_ids
        if websocket_ids.get(websocket) is None:
          websocket_ids[websocket] = all_new_ids
        else:
          websocket_ids[websocket].extend(all_new_ids)

        await websocket.send_text(json.dumps({
          "message": "register_batch",
          "ids": all_new_ids
        }))
      '''

      if message["type"] == "register_batch":
        number = message["number"]
        BATCH_SIZE = 1000  # оптимальный размер пачки для вставки
        decisions = [random.choice(['new', 'existing']) for _ in range(number)]
    
        all_ids = [None] * number
        # Сначала обработаем всех, кому нужен новый ID
        new_indices = [i for i, d in enumerate(decisions) if d == 'new']
        with get_db() as conn:
            with conn.cursor() as cur:
                # Вставляем новых пачками по BATCH_SIZE
                for batch_start in range(0, len(new_indices), BATCH_SIZE):
                    batch_end = min(batch_start + BATCH_SIZE, len(new_indices))
                    batch_size = batch_end - batch_start
                    values = ','.join(['(NOW(), NOW(), TRUE)'] * batch_size)
                    query = f"INSERT INTO players (with_us_since, last_login, is_playing) VALUES {values} RETURNING id"
                    cur.execute(query)
                    new_ids = [row[0] for row in cur.fetchall()]
                    # Раскладываем полученные ID по соответствующим позициям
                    for offset, bid in enumerate(new_ids):
                        all_ids[new_indices[batch_start + offset]] = bid
    
                # Теперь обработаем тех, кому нужен существующий свободный игрок
                existing_indices = [i for i, d in enumerate(decisions) if d == 'existing']
                if existing_indices:
                    # Выбираем свободных игроков в количестве len(existing_indices)
                    cur.execute("""
                        SELECT id FROM players
                        WHERE is_playing = FALSE
                        ORDER BY RANDOM()
                        LIMIT %s
                        FOR UPDATE SKIP LOCKED
                    """, (len(existing_indices),))
                    existing_ids = [row[0] for row in cur.fetchall()]
                    # Если свободных оказалось меньше, чем нужно, создаём новых для остальных
                    if len(existing_ids) < len(existing_indices):
                        # Добираем новых
                        need_new = len(existing_indices) - len(existing_ids)
                        values = ','.join(['(NOW(), NOW(), TRUE)'] * need_new)
                        query = f"INSERT INTO players (with_us_since, last_login, is_playing) VALUES {values} RETURNING id"
                        cur.execute(query)
                        extra_ids = [row[0] for row in cur.fetchall()]
                        existing_ids.extend(extra_ids)
    
                    # Обновляем статус is_playing и last_login для выбранных существующих
                    cur.executemany("""
                        UPDATE players SET is_playing = TRUE, last_login = NOW()
                        WHERE id = %s
                    """, [(bid,) for bid in existing_ids])
    
                    # Раскладываем ID по индексам
                    for idx, bid in zip(existing_indices, existing_ids):
                        all_ids[idx] = bid
    
        # Сохраняем все ID для этого сокета
        websocket_ids.setdefault(websocket, []).extend(all_ids)
    
        await websocket.send_text(json.dumps({
            "message": "register_batch",
            "ids": all_ids
        }))
      
      if message["type"] == "set_is_playing_tetris_batch":
        updates = message["data"]
        for u in updates:
          value = u["value"]
          is_playing_tetris_state[u["id"]] = u["value"]

      if message["type"] == "increase_number_of_sessions":
        with get_db() as conn:
          with conn.cursor() as cur:
            cur.execute(f"""
              UPDATE metrics 
              SET metric_value = metric_value + {message["data"]} 
              WHERE metric_name = 'total_sessions'
            """)

      if message["type"] == "update_metrics":
        data = message["data"]
        params = [(item["wins"], item["losses"], item["max_streak"], item["id"]) for item in data]
        with get_db() as conn:
          with conn.cursor() as cur:
            cur.executemany("""
              UPDATE players
              SET wins = wins + %s, losses = losses + %s, max_streak = GREATEST(max_streak, %s)
              WHERE id = %s
            """, params)

  except Exception as e:
    pass
  finally:
    if websocket_ids.get(websocket) is not None:
      with get_db() as conn:
        with conn.cursor() as cur:
          for id in websocket_ids[websocket]:
            cur.execute("""
              UPDATE players
              SET is_playing = FALSE
              WHERE id = %s
            """, (id,))
      del websocket_ids[websocket]


    if websocket in active_connections:
      active_connections.remove(websocket)


async def collect_ccu():
  while True:
    try:
      with get_db() as conn:
        with conn.cursor() as cur:
          cur.execute("SELECT COUNT(*) FROM players WHERE is_playing = TRUE AND is_playing_tetris = TRUE")
          ccu_value = cur.fetchone()[0]
          # dont work (problems with disconnect) ccu_value = is_playing_tetris_number
          cur.execute("INSERT INTO ccu (ts, value) VALUES (NOW(), %s)", (ccu_value,))
    except Exception as e:
      print(f"CCU error: {e}")

    await asyncio.sleep(5)

async def flush_is_playing_tetris_state():
  while True:
    await asyncio.sleep(5)
    if not is_playing_tetris_state:
      continue
    snapshot = is_playing_tetris_state.copy()
    is_playing_tetris_state.clear()

    with get_db() as conn:
      with conn.cursor() as cur:
        cur.executemany("""
          UPDATE players SET is_playing_tetris = %s WHERE id = %s
        """, [(v, k) for k, v in snapshot.items()])


@app.on_event("startup")
async def startup_event():
  asyncio.create_task(collect_ccu())
  asyncio.create_task(flush_is_playing_tetris_state())


@app.get("/tetris_bi/get_ccu_data")
def get_ccu_data(limit: int = 100):
  with get_db() as conn:
    with conn.cursor() as cur:
      cur.execute("""
        SELECT ts, value
        FROM ccu
        ORDER BY ts ASC
        LIMIT %s
      """, (limit,))
      rows = cur.fetchall()

  xs = [row[0].isoformat() for row in rows]
  ys = [row[1] for row in rows]

  return JSONResponse(content={
    "xs": xs,
    "ys": ys,
    "total_points": len(xs)
  })

@app.get("/tetris_bi/total_sessions")
def total_sessions():
    with get_db() as conn:
      with conn.cursor() as cur:
        cur.execute("SELECT metric_value FROM metrics WHERE metric_name = 'total_sessions'")
        row = cur.fetchone()
        count = row[0] if row else 0
    return JSONResponse(content={"total": count})

@app.get("/tetris_bi/wins_losses")
def wins_losses():
    with get_db() as conn:
      with conn.cursor() as cur:
        cur.execute("SELECT SUM(wins), SUM(losses) FROM players")
        wins, losses = cur.fetchone()
        wins = wins or 0
        losses = losses or 0
    return JSONResponse(content={"wins": wins,
                                 "losses": losses})

@app.get("/tetris_bi/rolling_retention_seconds")
def rolling_retention_seconds(bucket_seconds: int = 20, num_buckets: int = 10):
    with get_db() as conn:
        with conn.cursor() as cur:
            result = []
            cur.execute("SELECT COUNT(*) FROM players")
            total = cur.fetchone()[0]
            if total == 0:
                return JSONResponse(content=[])
            for k in range(num_buckets):
                threshold_seconds = k * bucket_seconds
                cur.execute("""
                    SELECT COUNT(*) FROM players
                    WHERE EXTRACT(EPOCH FROM (last_login - with_us_since)) >= %s
                """, (threshold_seconds,))
                active = cur.fetchone()[0]
                pct = round(100.0 * active / total, 2)
                result.append({"bucket": k, "retention_pct": pct})
    return JSONResponse(content=result)

@app.get("/tetris_bi/streaks_stats")
def streaks_stats():
    with get_db() as conn:
        with conn.cursor() as cur:
            cur.execute("SELECT MAX(max_streak) FROM players")
            max_val = cur.fetchone()[0] or 0
            cur.execute("SELECT AVG(max_streak) FROM players")
            avg_val = cur.fetchone()[0] or 0.0
            cur.execute("SELECT percentile_cont(0.5) WITHIN GROUP (ORDER BY max_streak) FROM players")
            median_val = cur.fetchone()[0] or 0.0
    return JSONResponse(content={
        "max": int(max_val) if max_val is not None else 0,
        "avg": float(avg_val) if avg_val is not None else 0.0,
        "median": float(median_val) if median_val is not None else 0.0
    })

app.mount("/static", StaticFiles(directory="static"), name="static")

from fastapi.responses import HTMLResponse

@app.get("/")
def root():
  with open("static/index.html", "r", encoding="utf-8") as f:
    return HTMLResponse(content=f.read())


if __name__ == "__main__":
  import uvicorn
  uvicorn.run(app, host="0.0.0.0", port=5837)
