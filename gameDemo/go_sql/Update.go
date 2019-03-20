package main

import (
	"database/sql"
    "os"
    "fmt"
	//"time"

	_ "github.com/go-sql-driver/mysql"
)

func main() {
	db, err := sql.Open("mysql", "zym:Zhao123456...@tcp(127.0.0.1:3306)/TestGo?charset=utf8")
	checkErr(err)

    id := os.Getenv("USERID")
    temp := os.Getenv("TAG")

    var tag [10]string
    for i := 0; i<10 ; i++ {
        tag[i] = string([]byte(temp)[i*10:i*10+10])
    }

	query_string := "UPDATE FORMATION SET tag0=?,tag1=?,tag2=?,tag3=?,tag4=?,tag5=?,tag6=?,tag7=?,tag8=?,tag9=? WHERE userid='"
    query_string += id
    query_string += "';"
    stmt, err := db.Prepare(query_string)
    checkErr(err)

    result, err := stmt.Exec(tag[0],tag[1], tag[2], tag[3], tag[4],tag[5], tag[6], tag[7], tag[8], tag[9])
    checkErr(err)

    affect, err := result.RowsAffected()
    checkErr(err)

    if affect == 1 {
        fmt.Print("OK")
    }else{
        fmt.Print("Fail")
    }
    fmt.Print(affect, query_string)

	db.Close()
}

func checkErr(err error) {
	if err != nil {
		panic(err)
	}
}
