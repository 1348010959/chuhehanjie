package main

import (
	"database/sql"
	"fmt"
	"os"
	//"time"

	_ "github.com/go-sql-driver/mysql"
)

func main() {
	db, err := sql.Open("mysql", "zym:Zhao123456...@tcp(127.0.0.1:3306)/TestGo?charset=utf8")
	checkErr(err)

	id := os.Getenv("USERID")
	pwd := os.Getenv("PASSWORD")
	//id := "123"
	//pwd := "123"

	query_string := "SELECT * FROM gameplayer WHERE userid='"
	query_string += id
	query_string += "';"
	rows, err := db.Query(query_string)
	checkErr(err)

	var flag byte
	for rows.Next() {
		var user_id string
		var user_name string
		var passwd string
		err = rows.Scan(&user_id, &passwd, &user_name)
		checkErr(err)
		if id == user_id && pwd == passwd {
			fmt.Print("OK",user_name)
		} else {
			flag = 0x11
			fmt.Print(flag)
		}
	}
	db.Close()
}

func checkErr(err error) {
	if err != nil {
		panic(err)
	}
}
