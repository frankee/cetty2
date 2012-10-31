DROP TABLE user_token;

CREATE TABLE user_token (
  username VARCHAR(50),
  host VARCHAR(100),
  nonce VARCHAR(20) NOT NULL,
  created VARCHAR(20),
  is_used TINYINT,
  PRIMARY KEY(username, created)
);
