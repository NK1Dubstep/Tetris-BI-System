import fastapi
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

app = fastapi.FastAPI()

@app.post("/tetris_bi/login_as_new_player")
def login_as_new_player():
  cur.execute("""
    INSERT INTO players (with_us_since, last_login, is_in_app, is_in_session)
    VALUES (NOW(), NOW(), TRUE, FALSE)
    RETURNING id
  """)
  conn.commit()

  try:
    id = cur.fetchone()[0]
    return JSONResponse(content={"message": "player successfully created", "id": id}, status_code=201)
  except:
    return JSONResponse(content={"message": "some error"}, status_code=400)

@app.post("/tetris_bi/login_as_free_player")
def login_as_free_player():
  cur.execute("""
    UPDATE players
    SET is_in_app = TRUE, last_login = NOW()
    WHERE id = (
      SELECT id FROM players
      WHERE is_in_app = FALSE
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

if __name__ == "__main__":
    import uvicorn
    uvicorn.run(app, host="0.0.0.0", port=5837)
