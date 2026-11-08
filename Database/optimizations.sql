CREATE INDEX IF NOT EXISTS idx_players_playing ON players(is_playing);
CREATE INDEX IF NOT EXISTS idx_ccu_ts ON ccu (ts);
CREATE INDEX IF NOT EXISTS idx_players_id ON players(id);
