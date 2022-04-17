#include<iostream>
#include<windows.h>
#include<conio.h>
#include<fstream>
#include<cstring>
#include<cstdio>
#include<cstdlib>
#include<iomanip>
#include<gtest/gtest.h>
using namespace std;
//global variable declaration
int k=7,r=0,flag=0;
COORD coord = {0, 0};

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

class item
{
    int itemno;
    char name[25];
    date d;
public:
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
    int retno()
    {
        return(itemno);
    }
};

class amount: public item
{
    float price,tax,gross,dis,netamt;
    int qty;
public:
    void add();
    void add(float price, int qty, float tax, float dis);
    void show();
    void report();
    void calculate();
    void pay();
    float retnetamt()
    {
        return(netamt);
    }
} amt;

void amount::add()
{
    item::add();
    cout<<"\n\n\tPrice: ";
    cin>>price;
    cout<<"\n\n\tQuantity: ";
    cin>>qty;
    cout<<"\n\n\tTax percent: ";
    cin>>tax;
    cout<<"\n\n\tDiscount percent: ";
    cin>>dis;
    calculate();
    fout.write((char *)&amt,sizeof(amt));
    fout.close();
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
    fin.open("itemstore.dat",ios::binary);
    fin.read((char*)&amt,sizeof(amt));
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

void amount::pay()
{
    show();
    cout<<"\n\n\n\t\t*********************************************";
    cout<<"\n\t\t                 DETAILS                  ";
    cout<<"\n\t\t*********************************************";
    cout<<"\n\n\t\tPRICE                     :"<<price;
    cout<<"\n\n\t\tQUANTITY                  :"<<qty;
    cout<<"\n\t\tTAX PERCENTAGE              :"<<tax;
    cout<<"\n\t\tDISCOUNT PERCENTAGE         :"<<dis;
    cout<<"\n\n\n\t\tNET AMOUNT              :Rs."<<netamt;
    cout<<"\n\t\t*********************************************\n";
}

void addRec() {
    fout.open("itemstore.dat", ios::binary | ios::app);
    amt.add();
    cout << "\n\t\tItem Added Successfully!";
    _getch();
}

void deleteRec() {
    int ino;
    flag = 0;
    cout << "\n\n\tEnter Item Number to be deleted :";
    cin >> ino;
    fin.open("itemstore.dat", ios::binary);
    if (!fin)
    {
        cout << "\n\nFile Not Found...";
        //goto menu;
        return;
    }
    //fstream tmp("temp.dat",ios::binary|ios::out);
    fin.seekg(0);
    while (fin.read((char*)&amt, sizeof(amt)))
    {
        int x = amt.item::retno();
        if (x != ino)
            tmp.write((char*)&amt, sizeof(amt));
        else
        {
            flag = 1;
        }
    }
    fin.close();
    tmp.close();
    fout.open("itemstore.dat", ios::trunc | ios::binary);
    fout.seekp(0);
    tmp.open("temp.dat", ios::binary | ios::in);
    if (!tmp)
    {
        cout << "Error in File";
        //goto db;
        return;
    }
    while (tmp.read((char*)&amt, sizeof(amt)))
        fout.write((char*)&amt, sizeof(amt));
    tmp.close();
    fout.close();
    if (flag == 1)
        cout << "\n\t\tItem Succesfully Deleted";
    else if (flag == 0)
        cout << "\n\t\tItem does not Exist! Please Retry";
}

int showRec(int cho)
{
    system("cls");
    flag = 0;
    fin.open("itemstore.dat", ios::binary);
    if (!fin)
    {
        cout << "\n\nFile Not Found...\nProgram Terminated!";
        //goto menu;
        return 0;
    }
    fin.seekg(0);
    while (fin.read((char*)&amt, sizeof(amt)))
    {
        int x = amt.item::retno();
        if (x == cho)
        {
            amt.pay();
            flag = 1;
            break;
        }
    }
    if (flag == 0)
        cout << "\n\t\tItem does not exist....Please Retry!";
    fin.close();
    return flag;
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
    return RUN_ALL_TESTS();

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
            system("cls");
            gotoxy(30,3);
            cout<<" BILL DETAILS ";
            gotoxy(3,5);
            cout<<"ITEM NO";
            gotoxy(13,5);
            cout<<"NAME";
            gotoxy(23,5);
            cout<<"PRICE";
            gotoxy(33,5);
            cout<<"QUANTITY";
            gotoxy(44,5);
            cout<<"TAX %";
            gotoxy(52,5);
            cout<<"DISCOUNT %";
            gotoxy(64,5);
            cout<<"NET AMOUNT";
            fin.open("itemstore.dat",ios::binary);
            if(!fin)
            {
                cout<<"\n\nFile Not Found...";
                goto menu;
            }
            fin.seekg(0);
            gtotal=0;
            while(!fin.eof())
            {
                fin.read((char*)&amt,sizeof(amt));
                if(!fin.eof())
                {
                    amt.report();
                    gtotal+=amt.retnetamt();
                    ff=0;
                }
                if(ff!=0) gtotal=0;
            }
            gotoxy(17,k);
            cout<<"\n\n\n\t\t\tGrand Total="<<gtotal;
            _getch();
            fin.close();
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
        cout<<"\t\t3.Back to Main Menu ";
        int apc;
        cin>>apc;
        switch(apc)
        {
        case 1:
            addRec();
            goto db;

        case 2:
            deleteRec();
            _getch();
            goto db;
        case 3:
            goto menu;
        default:
            cout<<"\n\n\t\tWrong Choice!!! Retry";
            _getch();
            goto db;
        }
    case 3:
        int ino;
        cout << "\n\n\t\tEnter Item Number :";
        cin >> ino;
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
