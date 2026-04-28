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

DB_CONFIG = {
    "dbname": "tetris_bi",
    "user": "postgres",
    "password": "123@",
    "host": "127.0.0.1",
    "port": "1492"
}

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

            if message["type"] == "register_batch":
                number = message["number"]

                # Подготовка данных: каждая строка — кортеж значений
                # Для повторяющихся значений (NOW(), NOW(), TRUE) можно сделать так:
                # Но NOW() — это функция БД, её нельзя передавать как значение.
                # Правильнее: вставить одну строку с вычислением NOW() на стороне БД,
                # либо передать одинаковое время из Python, либо использовать DEFAULT.
                # Я покажу два варианта.

                # Вариант А: вставить number строк, но с одинаковым значением NOW().
                # Для этого используем execute_values с явным указанием "NOW()" в шаблоне:
                insert_sql = """
                    INSERT INTO players (with_us_since, last_login, is_playing)
                    VALUES %s
                    RETURNING id
                """
                # Шаблон для одной строки: (NOW(), NOW(), TRUE) — это не плейсхолдеры, а текст
                # execute_values позволяет задать шаблон, куда подставляются значения.
                # Но т.к. значения не меняются, можно проще:

                # Вариант Б (рекомендую): генерируем простой INSERT без плейсхолдеров
                # При условии, что number не слишком огромное (безопасно)
                values = ','.join(['(NOW(), NOW(), TRUE)'] * number)
                query = f"INSERT INTO players (with_us_since, last_login, is_playing) VALUES {values} RETURNING id"

                with get_db() as conn:
                    with conn.cursor() as cur:
                        cur.execute(query)
                        new_ids = [row[0] for row in cur.fetchall()]
                conn = None  # после выхода из with соединение закрыто

                # Обновляем словарь websocket_ids
                if websocket_ids.get(websocket) is None:
                    websocket_ids[websocket] = new_ids
                else:
                    websocket_ids[websocket].extend(new_ids)

                await websocket.send_text(json.dumps({
                    "message": "register_batch",
                    "ids": new_ids
                }))

            #if message["type"] == "register_batch":
            #    number = message["number"]
#
            #    new_ids = []
#
            #    for i in range(number):
            #        with get_db() as conn:
            #            with conn.cursor() as cur:
            #                cur.execute("""
            #                    INSERT INTO players (with_us_since, last_login, is_playing)
            #                    VALUES (NOW(), NOW(), TRUE)
            #                    RETURNING id
            #                """)
            #                id = cur.fetchone()[0]
#
            #            new_ids.append(id)
            #            if websocket_ids.get(websocket) is None:
            #                websocket_ids[websocket] = [id]
            #            else:
            #                websocket_ids[websocket].append(id)
#
            #    await websocket.send_text(json.dumps({
            #        "message": "register_batch",
            #        "ids": new_ids
            #    }))


            if message["type"] == "set_is_playing_tetris_batch":
                updates = message["data"]
                for u in updates:
                    is_playing_tetris_state[u["id"]] = u["value"]



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
        with get_db() as conn:
            with conn.cursor() as cur:
                cur.executemany("""
                    UPDATE players SET is_playing_tetris = %s WHERE id = %s
                """, [("TRUE" if v else "FALSE", k) for k, v in snapshot.items()])


@app.on_event("startup")
async def startup_event():
    asyncio.create_task(collect_ccu())
    asyncio.create_task(flush_is_playing_tetris_state())

@app.get("/tetris_bi/set_is_playing_tetris")


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


app.mount("/static", StaticFiles(directory="static"), name="static")

from fastapi.responses import HTMLResponse

@app.get("/")
def root():
    with open("static/index.html", "r", encoding="utf-8") as f:
        return HTMLResponse(content=f.read())


if __name__ == "__main__":
    import uvicorn
    uvicorn.run(app, host="0.0.0.0", port=5837)
