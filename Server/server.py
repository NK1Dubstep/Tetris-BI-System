from fastapi import FastAPI, WebSocket
from fastapi.responses import JSONResponse
import json
import asyncio
from datetime import datetime
import psycopg2

conn = psycopg2.connect(
  dbname="tetris_bi",
  user="postgres",
  password="123@",
  host="127.0.0.1",
  port="1492"
)
cur = conn.cursor()

app = FastAPI()

@app.post("/tetris_bi/login_as_free_player")
def login_as_free_player():
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
  conn.commit()

  if not id:
    return JSONResponse(content={"message": "No free players found"}, status_code=404)

  return JSONResponse(content={"message": "free player successfully found", "id": id}, status_code=200)

app = FastAPI()

active_connections = []

websocket_ids = {}

@app.websocket("/ws")
async def websocket_endpoint(websocket: WebSocket):
  await websocket.accept()
  active_connections.append(websocket)
  player_id = None

  print(f"Client connected. Total: {len(active_connections)}")

  try:
    while True:
      data = await websocket.receive_text()
      message = json.loads(data)

      if message["type"] == "register":
        cur.execute("""
          INSERT INTO players (with_us_since, last_login, is_playing)
          VALUES (NOW(), NOW(), TRUE)
          RETURNING id
        """)
        conn.commit()
        id = cur.fetchone()[0]
        await websocket.send_text(json.dumps({
          "message": "OK",
          "id": id,
          "bot_index": message["bot_index"]
        }))
        ids = websocket_ids.get(websocket)
        if ids is None:
          websocket_ids[websocket] = [id]
        else:
          websocket_ids[websocket].append(id)
        print(websocket_ids[websocket])
        print(f"Player {id} registered")
      if message["type"] == "send_stats":
        print(message)
      if message["type"] == "set_is_playing_tetris":
        id = message["id"]
        is_playing_value = "FALSE" if message["value"] == 0 else "TRUE"

        cur.execute("""
          UPDATE players 
          SET is_playing_tetris = %s
          WHERE id = %s
        """, (is_playing_value, id))
        conn.commit()
        print(f"Player {id} is_playing_tetris set to {is_playing_value}")

  except Exception as e:
    print(f"Error: {e}")
  finally:
    if websocket_ids.get(websocket) is not None:
      for id in websocket_ids[websocket]:
        cur.execute("""
          UPDATE players
          SET is_playing = FALSE
          WHERE id = %s
        """, (id,))
        conn.commit()

      print(f"Player {id} disconnected, is_playing set to FALSE")

      del websocket_ids[websocket]

    if websocket in active_connections:
      active_connections.remove(websocket)
    print(f"Client disconnected. Total: {len(active_connections)}")

async def collect_ccu():
  while True:
    try:
      cur.execute("SELECT COUNT(*) FROM players WHERE is_playing = TRUE AND is_playing_tetris = TRUE")
      ccu_value = cur.fetchone()[0]
      print("COLLECT CCU FUNC ----> ", ccu_value)

      cur.execute("INSERT INTO ccu (ts, value) VALUES (NOW(), %s)", (ccu_value,))
      conn.commit()
    except Exception as e:
      print(f"CCU error: {e}")

    await asyncio.sleep(5)

@app.on_event("startup")
async def startup_event():
  asyncio.create_task(collect_ccu())

@app.get("/tetris_bi/get_ccu_data")
def get_ccu_data(limit: int = 100):
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

from fastapi.staticfiles import StaticFiles

app.mount("/static", StaticFiles(directory="static"), name="static")

@app.get("/")
def root():
    from fastapi.responses import HTMLResponse
    with open("static/index.html", "r", encoding="utf-8") as f:
        return HTMLResponse(content=f.read())

if __name__ == "__main__":

  loop = asyncio.new_event_loop()
  asyncio.set_event_loop(loop)
  loop.create_task(collect_ccu())

  import uvicorn
  uvicorn.run(app, host="0.0.0.0", port=5837)

