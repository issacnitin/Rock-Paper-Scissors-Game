#include <my_global.h>
#include <mysql.h>

int main(int argc, char **argv)
{  
  MYSQL *con = mysql_init(NULL);

  if (con == NULL) 
  {
      fprintf(stderr, "%s\n", mysql_error(con));
      exit(1);
  }

  if (mysql_real_connect(con, "localhost", "root", argv[1], 
          NULL, 0, NULL, 0) == NULL) 
  {
      fprintf(stderr, "%s\n", mysql_error(con));
      mysql_close(con);
      exit(1);
  }  

  if (mysql_query(con, "CREATE DATABASE rockpaperscissors")) 
  {
      fprintf(stderr, "%s\n", mysql_error(con));
     // mysql_close(con);
     // exit(1);
  }

  mysql_query(con, "create table rockpaperscissors.game(id INT NOT NULL AUTO_INCREMENT, player1 DOUBLE, player2 DOUBLE, PRIMARY KEY(id))"); 
  mysql_query(con, "create table rockpaperscissors.game_threads(id INT NOT NULL AUTO_INCREMENT, player1 DOUBLE, player2 DOUBLE, PRIMARY KEY(id))"); 
  
  printf("db setup complete\n");
  
  mysql_close(con);
  exit(0);
}