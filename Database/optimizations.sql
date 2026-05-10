CREATE INDEX IF NOT EXISTS idx_players_playing_tetris ON players(is_playing, is_playing_tetris) WHERE is_playing = TRUE;
CREATE INDEX IF NOT EXISTS idx_ccu_ts ON ccu (ts);
CREATE INDEX IF NOT EXISTS idx_players_id ON players(id);
