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
	name := os.Getenv("NAME")
	pwd := os.Getenv("PASSWORD")

	query_string := "SELECT * FROM gameplayer WHERE userid='"
	query_string += id
	query_string += "';"
	rows, err := db.Query(query_string)
	checkErr(err)

	if !rows.Next() {
		stmt, err := db.Prepare("INSERT INTO gameplayer SET userid=?,pwd=?,name=?")
		checkErr(err)

		_, err = stmt.Exec(id, pwd, name)
		checkErr(err)

        stmt2, err := db.Prepare("INSERT INTO FORMATION SET userid=?")
        checkErr(err)

        _, err = stmt2.Exec(id)
        checkErr(err)

		fmt.Print("OK")
	} else {
		fmt.Print("FAIL")
	}

	db.Close()
}

func checkErr(err error) {
	if err != nil {
		panic(err)
	}
}
