#include<iostream>
#include<windows.h>
#include<conio.h>
#include<fstream>
#include<cstring>
#include <string>
#include<cstdio>
#include<cstdlib>
#include<iomanip>
#include<gtest/gtest.h>
#include "sqlite3.h"
using namespace std;
//global variable declaration
int k = 7, r = 0, flag = 0, rc = 0; 
float global = 0;
sqlite3* db;
sqlite3_stmt* stmt;
COORD coord = {0, 0};
char* messageError;

void gotoxy(int x, int y)
{
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}
struct date
{
    int mm,dd,yy;
};

ofstream fout;
ifstream fin;
fstream tmp("temp.dat", ios::binary | ios::out);

int The_Callback(void* a_param, int argc, char** argv, char** column) {
    cout << "\t";
    for (int i = 0; i < argc; i++) {
        /*string a = argv[i];
        cout << a << endl;*/
        printf("%s,\t", argv[i]);
    }
    printf("\n");
    return 0;
}

int The_Sum_Callback(void* a_param, int argc, char** argv, char** column) {
    global = stof(argv[0]);
    return 0;
}

class item
{
    int itemno;
    char name[25];
    date d;
public:
    int getItemno() {
        return itemno;
    }
    string getName() {
        return name;
    }
    date getDate() {
        return d;
    }

    void add()
    {
        cout<<"\n\n\tItem No: ";
        cin>>itemno;
        cout<<"\n\n\tName of the item: ";
        cin>>name;
//gets(name);
        cout<<"\n\n\tManufacturing Date(dd-mm-yy): ";
        cin>>d.mm>>d.dd>>d.yy;
    }
    void show()
    {
        cout<<"\n\tItem No: ";
        cout<<itemno;
        cout<<"\n\n\tName of the item: ";
        cout<<name;
        cout<<"\n\n\tDate : ";
        cout<<d.mm<<"-"<<d.dd<<"-"<<d.yy;
    }
    void report()
    {
        gotoxy(3,k);
        cout<<itemno;
        gotoxy(13,k);
        puts(name);
    }
};

class amount: public item
{
    float price,tax,gross,dis,netamt;
    int qty;
public:
    float getPrice() {
        return price;
    }
    float getTax() {
        return tax;
    }
    float getGross() {
        return gross;
    }
    float getDis() {
        return dis;
    }
    float getNetamt() {
        return netamt;
    }
    int getQty() {
        return qty;
    }
    void collect();
    void add();
    void add(float price, int qty, float tax, float dis);
    void show();
    void report();
    void calculate();
} amt;


void amount::collect() {
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

void amount::add()
{
    collect();
    string sqlstatement = "INSERT INTO items (itemno, name, date, price, qty, tax, gross, dis, netamt) VALUES (" 
        + to_string(getItemno()) + ",'"
        + getName() + "','"
        + to_string(getDate().yy) + "-" + to_string(getDate().mm) + "-" + to_string(getDate().dd) + "'," 
        + to_string(this->price) + "," 
        + to_string(this->qty) + "," 
        + to_string(this->tax) + "," 
        + to_string(this->gross) + "," 
        + to_string(this->dis) + "," 
        + to_string(this->netamt) + ");";

    if (sqlite3_open("bill.db", &db) == SQLITE_OK)
    {
        sqlite3_exec(db, sqlstatement.c_str(), NULL, 0, &messageError);
    }
    else
    {
        cout << "Failed to open db\n";
    }
    //fout.write((char *)&amt,sizeof(amt));
    //fout.close();
}
void amount::add(float _price, int _qty, float _tax, float _dis) {
    price = _price;
    qty = _qty;

    tax = _tax;
    dis = _dis;
    calculate();
}
void amount::calculate()
{
    gross=price+(price*(tax/100));
    netamt=qty*(gross-(gross*(dis/100)));
}
void amount::show()
{
    item::show();
    cout<<"\n\n\tNet amount: ";
    cout<<netamt;
    fin.close();
}

void amount::report()
{
    item::report();
    gotoxy(23,k);
    cout<<price;
    gotoxy(33,k);
    cout<<qty;
    gotoxy(44,k);
    cout<<tax;
    gotoxy(52,k);
    cout<<dis;
    gotoxy(64,k);
    cout<<netamt;
    k=k+1;
    if(k==50)
    {
        gotoxy(25,50);
        cout<<"PRESS ANY KEY TO CONTINUE...";
        _getch();
        k=7;
        system("cls");
        gotoxy(30,3);
        cout<<" ITEM DETAILS ";
        gotoxy(3,5);
        cout<<"NUMBER";
        gotoxy(13,5);
        cout<<"NAME";
        gotoxy(23,5);
        cout<<"PRICE";
        gotoxy(33,5);
        cout<<"QUANTITY";
        gotoxy(44,5);
        cout<<"TAX";
        gotoxy(52,5);
        cout<<"DEDUCTION";
        gotoxy(64,5);
        cout<<"NET AMOUNT";
    }
}

void addRec() {
    //fout.open("itemstore.dat", ios::binary | ios::app);
    rc = sqlite3_open("bill.db", &db);

    amt.add();
    cout << "\n\t\tItem Added Successfully!";
    _getch();
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
    amt.collect();
    string sqlstatement = "UPDATE items SET itemno=" + to_string(amt.getItemno()) + ",name='" + amt.getName() + "',date='" + to_string(amt.getDate().yy) + "-" + to_string(amt.getDate().mm) + "-" + to_string(amt.getDate().dd) + "',price=" + to_string(amt.getPrice()) + ",qty=" + to_string(amt.getQty()) + ",tax=" + to_string(amt.getTax()) + ",gross=" + to_string(amt.getGross()) + ",dis=" + to_string(amt.getGross()) + ",netamt=" + to_string(amt.getNetamt()) + " where itemno=" + to_string(cho) + ";";
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

int showRec(int cho) {

    string sqlstatement = "SELECT * FROM items WHERE itemno=" + to_string(cho) + ";";

    if (sqlite3_open("bill.db", &db) == SQLITE_OK)
    {
        sqlite3_exec(db, sqlstatement.c_str(), The_Callback, 0, &messageError);
    }
    else
    {
        cout << "Failed to open db\n";
    }
    return 1;
}

TEST(ProgramTest, ShowItemTest) {
    int check = showRec(1);
    EXPECT_EQ(check, 1);
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
    cout<<setprecision(2);
menu:
    system("cls");
    gotoxy(25,2);
    cout<<"Super Market Billing ";
    gotoxy(25,3);
    cout<<"===========================\n\n";
    cout<<"\n\t\t1.Bill Report\n\n";
    cout<<"\t\t2.Add/Remove/Edit Item\n\n";
    cout<<"\t\t3.Show Item Details\n\n";
    cout<<"\t\t4.Exit\n\n";
    cout<<"\t\tPlease Enter Required Option: ";
    int ch,ff = 0;
    float gtotal;
    cin>>ch;
    switch(ch)
    {
    case 1:
ss:
        system("cls");
        gotoxy(25,2);
        cout<<"Bill Details";
        gotoxy(25,3);
        cout<<"================\n\n";
        cout<<"\n\t\t1.All Items\n\n";
        cout<<"\t\t2.Back to Main menu\n\n";
        cout<<"\t\tPlease Enter Required Option: ";
        int cho;
        cin>>cho;
        if(cho==1)
        {
            gtotal = 0;
            system("cls");
            gotoxy(30,3);
            cout<<" BILL DETAILS ";
            gotoxy(3,5);
            cout<<"ITEM NO";
            gotoxy(15,5);
            cout<<"NAME";
            gotoxy(23,5);
            cout<<"PRICE";
            gotoxy(30,5);
            cout<<"QUANTITY";
            gotoxy(39,5);
            cout<<"TAX %";
            gotoxy(45,5);
            cout<<"DISCOUNT %";
            gotoxy(56,5);
            cout<<"NET AMOUNT\n";

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

            cout<<"\n\n\n\t\t\tGrand Total="<<global;
            _getch();
        }
        if(cho==2)
        {
            goto menu;
        }
        goto ss;
    case 2:
db:
        system("cls");
        gotoxy(25,2);
        cout<<"Bill Editor";
        gotoxy(25,3);
        cout<<"=================\n\n";
        cout<<"\n\t\t1.Add Item Details\n\n";
        cout<<"\t\t2.Delete Item Details\n\n";
        cout << "\t\t3.Edit Item Details\n\n";
        cout<<"\t\t4.Back to Main Menu ";
        int apc;
        cin>>apc;
        switch(apc)
        {
        case 1:
            addRec();
            goto db;
        case 2:
            int ino;
            cout << "\n\n\tEnter Item Number to be deleted :";
            cin >> ino;
            deleteRec(ino);
            goto db;
        case 3:
            cout << "\n\n\t\tEnter Item Number: ";
            cin >> ino;
            editRec(ino);
            goto db;
        case 4:
            goto menu;
        default:
            cout<<"\n\n\t\tWrong Choice!!! Retry";
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
        cout << "DATE";
        gotoxy(40, 5);
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
        gotoxy(20,20);
        cout<<"ARE YOU SURE, YOU WANT TO EXIT (Y/N)?";
        char yn;
        cin>>yn;
        if((yn=='Y')||(yn=='y'))
        {
            gotoxy(12,20);
            system("cls");
            cout<<"************************** THANKS **************************************";
            _getch();
            exit(0);
        }
        else if((yn=='N')||(yn=='n'))
            goto menu;
        else
        {
            goto menu;
        }
    default:
        cout<<"\n\n\t\tWrong Choice....Please Retry!";
        _getch();
        goto menu;
    }
    return 0;
}
