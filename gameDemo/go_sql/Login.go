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


	for rows.Next() {
		var user_id string
		var user_name string
		var passwd string
		err = rows.Scan(&user_id, &passwd, &user_name)
		checkErr(err)
		if id == user_id && pwd == passwd {
			fmt.Print("OK",user_name)
            Tags(id, db)
		} else {
			fmt.Print("FAIL")
		}
	}
	db.Close()
}

func Tags(id string, db *sql.DB){
    query_Match := "SELECT * FROM FORMATION WHERE userid='"
    query_Match += id
    query_Match += "';'"
    Matchrows, err := db.Query(query_Match)
    checkErr(err)

    for Matchrows.Next() {
        var tags [10]int
        err = Matchrows.Scan(&tags[0],&tags[1],&tags[2],&tags[3],&tags[4],&tags[5],&tags[6],&tags[7],&tags[8],&tags[9])
        fmt.Printf("%d",tags[0])
        fmt.Printf("%d",tags[1])
        fmt.Printf("%d",tags[2])
        fmt.Printf("%d",tags[3])
        fmt.Printf("%d",tags[4])
        fmt.Printf("%d",tags[5])
        fmt.Printf("%d",tags[6])
        fmt.Printf("%d",tags[7])
        fmt.Printf("%d",tags[8])
        fmt.Printf("%d",tags[9])
    }
}

func checkErr(err error) {
	if err != nil {
		panic(err)
	}
}
