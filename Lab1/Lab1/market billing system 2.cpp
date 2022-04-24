/// @file

#include<iostream>
#include<windows.h>
#include<conio.h>
#include<cstring>
#include<string>
#include <sstream>
#include<cstdio>
#include<cstdlib>
#include<iomanip>
#include<gtest/gtest.h>
#include "sqlite_lib/sqlite3.h"
using namespace std;

int k = 7, r = 0, flag = 0, rc = 0;
float global = 0;
sqlite3* db;
sqlite3_stmt* stmt;
COORD coord = { 0, 0 };
char* messageError;

/// \brief Represents day, month and year
/// 
/// @param dd day
/// @param mm month
/// @param yy year
class date
{
    int mm, dd, yy = 0;
public:
    /// Getter for month
    /// 
    /// @returns month (mm)
    int getMonth() {
        return mm;
    }

    /// Getter for day
    /// 
    /// @returns day (dd)
    int getDay() {
        return dd;
    }

    /// Getter for year
    /// 
    /// @returns year (yy)
    int getYear() {
        return yy;
    }

    /// Setter for month
    /// 
    /// Sets the value of mm parameter
    /// @param month value which will be set
    /// @returns nothing
    void setMonth(int month) {
        mm += month;
    }

    /// Setter for day
    /// 
    /// Sets the value of dd parameter
    /// @param day value which will be set
    /// @returns nothing
    void setDay(int day) {
        dd += day;
    }

    /// Setter for year
    /// 
    /// Sets the value of yy parameter
    /// @param year value which will be set
    /// @returns nothing
    void setYear(int year) {
        yy += year;
    }
};

/// \brief Product and it's info
/// 
/// @param itemno - special id for this item
/// @param name - How this item is called
/// @param d - date when item was manufactured
class item
{
    int itemno;
    string name;
    date d;
public:
    /// \brief Getter for itemno variable
    /// 
    /// @returns itemno
    int getItemno() {
        return itemno;
    }

    /// \brief Getter for name variable
    /// 
    /// @returns name
    string getName() {
        return name;
    }

    /// \brief Getter for d variable
    /// 
    /// @returns d (date)
    date getDate() {
        return d;
    }

    /// \brief Processes the input from user
    /// 
    /// @returns nothing
    void add()
    {
        int month, day, year = 0;

    itno:
        cout << "\n\n\tItem No: ";
        cin >> itemno;
        if (cin.fail()) {
            cout << "Value has to be a number!";
            cin.clear();
            cin.ignore();
            goto itno;
        }
        cin.ignore();
        cout << "\n\n\tName of the item: ";
        getline(cin, name);
        cin.clear();
        cout << "\n\n\tManufacturing Date(dd-mm-yy): ";
    di:
        cin >> day >> month >> year;
        if (day > 31 || day <= 0 || month <= 0 || month > 12) {
            cout << "Wrong input!";
            goto di;
        }
        d.setMonth(month);
        d.setDay(day);
        d.setYear(year);
        cin.clear();
    }
};

/// Represents a cost of an item
///     
/// @param price Amount of money custumer need to purchase the item without discount or tax.
/// @param tax a Compulsory contribution to state revenue.
/// @param gross Difference between revenue and the cost of product.
/// @param dis Discount. A deduction from the usual cost of product.
/// @param netamt Overall cost of product for a customer.
/// @param qty Quantity (amount) of particullar item customer wants to buy.
class amount : public item
{
    double price, tax, gross, dis, netamt = 0.0;
    int qty = 0;
public:
    /// \brief Getter for *price* variable.
    /// 
    /// @returns *price*
    double getPrice() {
        return price;
    }

    /// \brief Getter for *tax* variable.
    /// 
    /// @returns *tax*
    double getTax() {
        return tax;
    }

    /// \brief Getter for *gross* variable.
    /// 
    /// @returns *gross*
    double getGross() {
        return gross;
    }

    /// \brief Getter for *dis* variable.
    /// 
    /// @returns *dis*
    double getDis() {
        return dis;
    }

    /// \brief Getter for *netamt* variable.
    /// 
    /// @returns *netamt*
    double getNetamt() {
        return netamt;
    }

    /// \brief Getter for *qty* variable.
    /// 
    /// @returns *qty*
    int getQty() {
        return qty;
    }

    /// \brief Collect info from user's input for both item and amount classes.
    /// 
    /// User has to input some varibles.
    /// @returns nothing
    void add();

    /// \brief Simmilar to add(), but parameters are passed right to the funtion, instead of input.
    /// 
    /// Used to test the calculation function.
    /// @returns nothing
    void add(double price, int qty, double tax, double dis);

    /// \brief Calculates the **netamt** and **gross** variables based on other variables.
    /// 
    /// @returns nothing
    void calculate();
} amt;

void amount::add() {
    item::add();
    cout << "\n\n\tPrice: ";
    cin >> price;
    cout << "\n\n\tQuantity: ";
    cin >> qty;
    cout << "\n\n\tTax percent: ";
    cin >> tax;
    cout << "\n\n\tDiscount percent: ";
    cin >> dis;
    calculate();
}

void amount::add(double _price, int _qty, double _tax, double _dis) {
    price = _price;
    qty = _qty;

    tax = _tax;
    dis = _dis;
    calculate();
}

void amount::calculate()
{
    gross = price + (price * (tax / 100));
    netamt = qty * (gross - (gross * (dis / 100)));
}

void gotoxy(int x, int y)
{
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

int The_Callback(void* a_param, int argc, char** argv, char** column) {
    cout << "\t";
    for (int i = 0; i < argc; i++) {
        printf("%s,\t", argv[i]);
    }
    printf("\n");
    return 0;
}

int The_Sum_Callback(void* a_param, int argc, char** argv, char** column) {
    global = stof(argv[0]);
    return 0;
}

void addRec() {
    rc = sqlite3_open("bill.db", &db);

    amt.add();
    string sqlstatement = "INSERT INTO items (itemno, name, date, price, qty, tax, gross, dis, netamt) VALUES ("
        + to_string(amt.getItemno()) + ",'"
        + amt.getName() + "','"
        + to_string(amt.getDate().getYear()) + "-" + to_string(amt.getDate().getMonth()) + "-" + to_string(amt.getDate().getDay()) + "',"
        + to_string(amt.getPrice()) + ","
        + to_string(amt.getQty()) + ","
        + to_string(amt.getTax()) + ","
        + to_string(amt.getGross()) + ","
        + to_string(amt.getDis()) + ","
        + to_string(amt.getNetamt()) + ");";

    if (sqlite3_open("bill.db", &db) == SQLITE_OK)
    {
        sqlite3_exec(db, sqlstatement.c_str(), NULL, 0, &messageError);
    }
    else
    {
        cout << "Failed to open db\n";
    }

    cout << "\n\t\tItem Added Successfully!";
}

void deleteRec(int cho) {
    string sqlstatement = "DELETE FROM items where itemno=" + to_string(cho) + ";";
    if (sqlite3_open("bill.db", &db) == SQLITE_OK)
    {
        sqlite3_exec(db, sqlstatement.c_str(), NULL, 0, &messageError);
    }
    else
    {
        cout << "Failed to open db\n";
    }
}

void editRec(int cho) {
    amt.add();
    string sqlstatement = "UPDATE items SET itemno=" + to_string(amt.getItemno()) + ",name='" + amt.getName() + "',date='" + to_string(amt.getDate().getYear()) + "-" + to_string(amt.getDate().getMonth()) + "-" + to_string(amt.getDate().getDay()) + "',price=" + to_string(amt.getPrice()) + ",qty=" + to_string(amt.getQty()) + ",tax=" + to_string(amt.getTax()) + ",gross=" + to_string(amt.getGross()) + ",dis=" + to_string(amt.getGross()) + ",netamt=" + to_string(amt.getNetamt()) + " where itemno=" + to_string(cho) + ";";
    if (sqlite3_open("bill.db", &db) == SQLITE_OK)
    {
        sqlite3_exec(db, sqlstatement.c_str(), NULL, 0, &messageError);
        sqlite3_close(db);
    }
    else
    {
        cout << "Failed to open db\n";
    }
}

void showRec(int cho) {

    string sqlstatement = "SELECT * FROM items WHERE itemno=" + to_string(cho) + ";";

    if (sqlite3_open("bill.db", &db) == SQLITE_OK)
    {
        sqlite3_exec(db, sqlstatement.c_str(), The_Callback, 0, &messageError);
    }
    else
    {
        cout << "Failed to open db\n";
    }
}

TEST(ProgramTest, PriceCalculation) {
    amt.add(100, 1, 0, 0);
    EXPECT_EQ(amt.getNetamt(), 100);
    amt.add(200, 1, 0, 0);
    EXPECT_EQ(amt.getNetamt(), 200);
    amt.add(1000, 1, 0, 0);
    EXPECT_EQ(amt.getNetamt(), 1000);
    amt.add(2059.230, 1, 0, 0);
    EXPECT_EQ(amt.getNetamt(), 2059.23);
}
TEST(ProgramTest, QtyCalculation) {
    amt.add(100, 2, 0, 0);
    EXPECT_EQ(amt.getNetamt(), 200);
    amt.add(100, 30, 0, 0);
    EXPECT_EQ(amt.getNetamt(), 3000);
}
TEST(ProgramTest, TaxCalcualtion) {
    amt.add(100, 1, 50, 0);
    EXPECT_EQ(amt.getNetamt(), 150);
    amt.add(100, 1, 200, 0);
    EXPECT_EQ(amt.getNetamt(), 300);
}
TEST(ProgramTest, DisCalcualtion) {
    amt.add(100, 1, 0, 50);
    EXPECT_EQ(amt.getNetamt(), 50);
    amt.add(100, 1, 0, 70);
    EXPECT_EQ(amt.getNetamt(), 30);
}
TEST(ProgramTest, OverallCalcualtion) {
    amt.add(100, 2, 50, 0);
    EXPECT_EQ(amt.getNetamt(), 300);
    amt.add(100, 3, 60, 0);
    EXPECT_EQ(amt.getNetamt(), 480);
    amt.add(100, 3, 60, 0);
    EXPECT_EQ(amt.getNetamt(), 480);
    amt.add(100, 3, 60, 40);
    EXPECT_EQ(amt.getNetamt(), 288);
    amt.add(200, 5, 40, 75);
    EXPECT_EQ(amt.getNetamt(), 350);
}
TEST(ProgramTest, ShowItemTest) {
    SUCCEED(showRec(1));
}
TEST(ProgramTest, AddItemTest) {
    SUCCEED(addRec());
}
TEST(ProgramTest, DeleteItemTest) {
    SUCCEED(deleteRec());
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    //return RUN_ALL_TESTS();

    cout.setf(ios::fixed);
    cout.setf(ios::showpoint);
    cout << setprecision(2);
menu:
    system("cls");
    gotoxy(25, 2);
    cout << "Super Market Billing ";
    gotoxy(25, 3);
    cout << "===========================\n\n";
    cout << "\n\t\t1.Bill Report\n\n";
    cout << "\t\t2.Add/Remove/Edit Item\n\n";
    cout << "\t\t3.Show Item Details\n\n";
    cout << "\t\t4.Exit\n\n";
    cout << "\t\tPlease Enter Required Option: ";
    int ch, ff = 0;
    float gtotal;
    cin >> ch;
    switch (ch)
    {
    case 1:
    ss:
        system("cls");
        gotoxy(25, 2);
        cout << "Bill Details";
        gotoxy(25, 3);
        cout << "================\n\n";
        cout << "\n\t\t1.All Items\n\n";
        cout << "\t\t2.Back to Main menu\n\n";
        cout << "\t\tPlease Enter Required Option: ";
        int cho;
        cin >> cho;
        if (cho == 1)
        {
            gtotal = 0;
            system("cls");
            gotoxy(30, 3);
            cout << " BILL DETAILS ";
            gotoxy(8, 5);
            cout << "ITEM NO";
            gotoxy(20, 5);
            cout << "NAME";
            gotoxy(28, 5);
            cout << "PRICE";
            gotoxy(35, 5);
            cout << "QUANTITY";
            gotoxy(44, 5);
            cout << "TAX %";
            gotoxy(50, 5);
            cout << "DISCOUNT %";
            gotoxy(61, 5);
            cout << "NET AMOUNT\n";

            string sqlstatement = "SELECT itemno, name, price, qty, tax, dis, netamt FROM items;";
            if (sqlite3_open("bill.db", &db) == SQLITE_OK)
            {
                sqlite3_exec(db, sqlstatement.c_str(), The_Callback, 0, &messageError);
            }
            else
            {
                cout << "Failed to open db\n";
            }

            sqlstatement = "SELECT SUM(netamt) FROM items;";
            sqlite3_exec(db, sqlstatement.c_str(), The_Sum_Callback, 0, &messageError);

            cout << "\n\n\n\t\t\tGrand Total=" << global;
            _getch();
        }
        if (cho == 2)
        {
            goto menu;
        }
        goto ss;
    case 2:
    db:
        system("cls");
        gotoxy(25, 2);
        cout << "Bill Editor";
        gotoxy(25, 3);
        cout << "=================\n\n";
        cout << "\n\t\t1.Add Item Details\n\n";
        cout << "\t\t2.Delete Item Details\n\n";
        cout << "\t\t3.Edit Item Details\n\n";
        cout << "\t\t4.Back to Main Menu ";
        int apc;
        cin >> apc;
        switch (apc)
        {
        case 1:
            addRec();
            _getch();
            goto db;
        case 2:
            int ino;
            cout << "\n\n\tEnter Item Number to be deleted :";
            cin >> ino;
            deleteRec(ino);
            _getch();
            goto db;
        case 3:
            cout << "\n\n\t\tEnter Item Number: ";
            cin >> ino;
            editRec(ino);
            _getch();
            goto db;
        case 4:
            goto menu;
        default:
            cout << "\n\n\t\tWrong Choice!!! Retry";
            _getch();
            goto db;
        }
    case 3:
        int ino;
        cout << "\n\n\t\tEnter Item Number: ";
        cin >> ino;
        system("cls");
        gotoxy(30, 3);
        cout << " ITEM DETAILS ";
        gotoxy(3, 5);
        cout << "ITEM NO";
        gotoxy(15, 5);
        cout << "NAME";
        gotoxy(24, 5);
        cout << "DATE(y-m-d)";
        gotoxy(39, 5);
        cout << "PRICE";
        gotoxy(47, 5);
        cout << "QUANTITY";
        gotoxy(56, 5);
        cout << "TAX %";
        gotoxy(64, 5);
        cout << "GROSS";
        gotoxy(71, 5);
        cout << "DISCOUNT %";
        gotoxy(80, 5);
        cout << "NET AMOUNT\n";
        showRec(ino);
        _getch();
        goto menu;
    case 4:
        system("cls");
        gotoxy(20, 20);
        cout << "ARE YOU SURE, YOU WANT TO EXIT (Y/N)?";
        char yn;
        cin >> yn;
        if ((yn == 'Y') || (yn == 'y'))
        {
            gotoxy(12, 20);
            system("cls");
            cout << "************************** THANKS **************************************";
            _getch();
            exit(0);
        }
        else if ((yn == 'N') || (yn == 'n'))
            goto menu;
        else
        {
            goto menu;
        }
    default:
        cout << "\n\n\t\tWrong Choice....Please Retry!";
        _getch();
        goto menu;
    }
    return 0;
}

