DROP TABLE user_roles;

CREATE TABLE user_roles (
  username VARCHAR(50) ,
  role_name VARCHAR(50),
  PRIMARY KEY(username, role_name)
);

INSERT INTO user_roles VALUES('chenhl', 'R1');
INSERT INTO user_roles VALUES('chenhl', 'R2');
INSERT INTO user_roles VALUES('zhang', 'R2');
INSERT INTO user_roles VALUES('zhang', 'R3');


