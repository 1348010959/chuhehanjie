package main

import (
    "database/sql"
    "fmt"
    "os"
    //"time"

    _ "github.com/go-sql-driver/mysql"
)


func main() {
    db, err := sql.Open("mysql", "zym:Zhao123456@tcp(39.105.149.213:3306)/TestGo?charset=utf8")
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
            result := Tags(user_id, db)
            fmt.Printf("OK%s%s", result, user_name)
        } else {
            fmt.Print("FAIL")
        }
    }
    db.Close()
}

func Tags(id string, db *sql.DB) string{
    query_Match := "SELECT * FROM FORMATION WHERE userid='"
    query_Match += id
    query_Match += "';"
    Matchrows, err := db.Query(query_Match)
    checkErr(err)
    var tag0 string
    var tag1 string
    var tag2 string
    var tag3 string
    var tag4 string
    var tag5 string
    var tag6 string
    var tag7 string
    var tag8 string
    var tag9 string
    for Matchrows.Next() {
        err = Matchrows.Scan(&id, &tag0,&tag1,&tag2,&tag3,&tag4,&tag5,&tag6,&tag7,&tag8,&tag9)
    }
    var result string
    result += tag0
    result += tag1
    result += tag2
    result += tag3
    result += tag4
    result += tag5
    result += tag6
    result += tag7
    result += tag8
    result += tag9
    return result
}


func checkErr(err error) {
    if err != nil {
        panic(err)
    }
}
