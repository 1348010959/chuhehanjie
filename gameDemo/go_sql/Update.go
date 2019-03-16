package main

import (
	"database/sql"
	"os"
	//"time"

	_ "github.com/go-sql-driver/mysql"
)

func main() {
	db, err := sql.Open("mysql", "zym:Zhao123456...@tcp(127.0.0.1:3306)/TestGo?charset=utf8")
	checkErr(err)

	id := os.Getenv("USERID")
    tag := os.Getenv("TAG")

	query_string := "UPDATE FORMATION SET tag0=?,tag1=?,tag2=?,tag3=?,tag4=?,tag5=?,tag6=?,tag7=?,tag8=?,tag9=?   WHERE userid='"
    query_string += id
    query_string += ";'"
    stmt, err := db.Prepare(query_string)
    checkErr(err)

    stmt.Exec(tag)

	db.Close()
}

func checkErr(err error) {
	if err != nil {
		panic(err)
	}
}
