DROP TABLE roles_permissions;

CREATE TABLE roles_permissions (
  role_name VARCHAR(50) ,
  permission VARCHAR(50),
  PRIMARY KEY(role_name, permission)
);

INSERT INTO roles_permissions VALUES('R1', 'A:B:C');
INSERT INTO roles_permissions VALUES('R1', 'W:R');
INSERT INTO roles_permissions VALUES('R2', 'A:B:C:D');
INSERT INTO roles_permissions VALUES('R2', 'D:W:R');
INSERT INTO roles_permissions VALUES('R3', 'A:B:C:D:E');
INSERT INTO roles_permissions VALUES('R3', 'D:R');

