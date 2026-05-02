CREATE TABLE IF NOT EXISTS metrics (
    metric_name VARCHAR(50) PRIMARY KEY,
    metric_value BIGINT DEFAULT 0
);

INSERT INTO metrics (metric_name, metric_value) 
VALUES ('total_sessions', 0)
ON CONFLICT (metric_name) DO NOTHING;