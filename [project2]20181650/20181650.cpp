#define _CRT_SECURE_NO_WARNINGS
#define MAXVAL 1e9
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include "mysql.h"
using namespace std;

#pragma comment(lib, "libmysql.lib")

const char* host = "localhost";
const char* user = "root";
const char* pw = "20181650ehgus";
const char* db = "project";
string pastyear = "2021";	//�۳� ����
string year_for_pastmonth = "2022";	//1���� ��� �������� ���� �۳��� �⵵ ����
string month_for_pastmonth = "5";	//���� �� ����

//selection message�� ����ϰ� stdin���� �Է¹޾� �� ���� �����ϴ� �Լ�
int putSelectMessage() {	
	int input;
	cout << "\n---------- SELECT QUERY TYPES ----------\n\n";
	cout << "\t1. TYPE 1\n" << "\t2. TYPE 2\n" << "\t3. TYPE 3\n" << "\t4. TYPE 4\n" << "\t5. TYPE 5\n"
		<< "\t6. TYPE 6\n" << "\t7. TYPE 7\n" << "\t0. QUIT\n\n";
	cout << "USER INPUT: ";
	cin >> input;
	return input;
}
//sql_result�κ��� ���� ����� stdout�� ���. �ι�° ���ڴ� �� ����� Ʃ���� ������, ����Ʈ ���ڷ� ū �� ��ũ�θ� �ش�.
void query_print(MYSQL_RES* sql_result, int n=MAXVAL) {
	MYSQL_ROW sql_row;
	int num_fields;
	num_fields = mysql_num_fields(sql_result);	//�ʵ�(�Ӽ�)�� ����
	while ((sql_row = mysql_fetch_row(sql_result)) != NULL && n > 0)
	{
		n--;
		for (int i = 0; i < num_fields; i++) {
			if (sql_row[i])
				cout << sql_row[i] << " | ";
			else
				cout << "NULL" << " | ";	//�Ӽ��� NULL�� �� ��� ����� ���� �Ͽ� ������ ����
		}
		cout << '\n';
	}
	mysql_free_result(sql_result);
}
int main(void) {
	//BASIC CONNECTION ROUTINE
	MYSQL* connection = NULL;
	MYSQL conn;
	MYSQL_RES* sql_result;
	MYSQL_ROW sql_row;
	char* temp = new char[10];
	if (mysql_init(&conn) == NULL)
		cout << "mysql_init() error!";
	connection = mysql_real_connect(&conn, host, user, pw, db, 3306, (const char*)NULL, 0);
	if (connection == NULL)
	{
		printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));
		return 1;
	}

	cout << "Connection Succeed" << '\n';
	if (mysql_select_db(&conn, db))
	{
		printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));
		return 1;
	}
	//CONNECTION ROUTINE ENDS

	string buf;	//sql ó���� ���� ����
	int state,num_fields;	//state=���� ��û �Լ��� ���ϰ�, num_fields=�ʵ��� ���� ����
	ifstream fin("20181650_create_insert.txt");	//Create�� Insert�� ���� �ִ� �Է����� open
	if (fin.fail()) {
		cerr << "create&insert txt file open error!\n";
		return 1;
	}
	while (!fin.eof()) {
		getline(fin, buf,';');
		cout << buf;
		if (buf.empty())
			continue;
		state = mysql_query(connection, buf.c_str());
		if (state)
			printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));
		buf.clear();
	}
	fin.close();

	//query routine starts
	while (1) {
		int input = putSelectMessage();	//selection message �ݺ� ���
		if (input == 0) {	//0 input: break-> delete/drop ����
			cout << "---- User Requested QUIT! ----\n\n";
			break;
		}
		else if (input == 1) {	//1 input: tracking_num�� �Է¹޾� �׷κ��� ����� �ֹ� �����ο� ������ ���� ����ó�� ����.
			cout << "---- TYPE 1 ----\n\n";
			string x;
			cout << "Input tracking_num: ";
			cin >> x;
			//�ڵ� ��ü���� ���� ������ join. join�� ���� �׹��̳� �̷�������� redundancy�� �ּ�ȭ�� �������� �Ѱ��� ���̴�.
			//�ʿ��� ������ �� id�� �̸�, ����ó �Ӽ��� ǥ���� ���ü� ����
			buf = "select customer_id,customer_name,phone_num from ((shipping natural join online_order)inner join _order using (order_id)) inner join customer using (customer_id) where tracking_num = \'" + x + "\'";
			state = mysql_query(connection, buf.c_str());
			if (state) {
				cerr << "TYPE 1 Error!\n";
				printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));
				continue;
			}
			cout << "customer_id | customer_name | contact\n";
			cout << "-------------------------------------\n";
			sql_result = mysql_store_result(connection);
			query_print(sql_result);

			int subinput;
			while (1) {
				cout << "----------Subtypes in TYPE 1 ----------\n\n";
				cout << "\t1. TYPE 1-1\n\t0. Back to Select menu\n\n";
				cout << "USER INPUT: ";
				cin >> subinput;
				//1-1 input: 1���� �Է¹��� ��� ������ tracking_num�� �ٲپ� shipping ���̺� ����, ��
				//������ ���� �ش� �ֹ��� tracking_num�� ���� �����Ѵ�.
				if (subinput == 1) { 
					cout << "-----TYPE 1-1 -----\n\n";
					buf = "select tracking_num from shipping";	//shipping ���̺��� Ʃ���� ������ ����ϱ� ���� ����
					state = mysql_query(connection, buf.c_str());
					sql_result = mysql_store_result(connection);
					sql_row = mysql_fetch_row(sql_result);
					int maxship = 100000 + mysql_num_rows(sql_result);	//base tracking_num=100000 + row���� = maxship
					maxship++;	//�� ��� ������ �ο��� tracking_num.
					_itoa(maxship, temp, 10);
					string tmp = temp;

					buf = "select * from shipping where tracking_num=\'" + x + "\'";	//1���� �Է¹��� �ѹ��� ������� ��ȸ
					state = mysql_query(connection, buf.c_str());
					sql_result = mysql_store_result(connection);
					sql_row = mysql_fetch_row(sql_result);
					cout << "tracking_num | company | address | isarrived | got_problem\n";
					cout << "----------------------------------------------------------\n";
					cout << sql_row[0] << " | " << sql_row[1] << " | " << sql_row[2] << " | " << sql_row[3] << " | " << sql_row[4] << '\n';
					mysql_free_result(sql_result);

					//���Ӱ� ������ ������� ����
					buf = "insert into shipping values(" + tmp + ",\'" + sql_row[1] + "\',\'" + sql_row[2] + "\'," + "0,0);";
					cout << "\nNow processing " << buf << '\n';
					state = mysql_query(connection, buf.c_str());
					if (state) {
						cerr << "TYPE 1-1 Error!\n";
						printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));
					}

					//�ֹ� �������� ������ ������ ���� tracking_num �ڸ��� ���ο� �ѹ��� �����Ѵ�.
					buf = "update online_order set tracking_num= " + tmp + " where tracking_num=" + x;
					cout << "\nNow processing " << buf << '\n';
					state = mysql_query(connection, buf.c_str());
					if (state) {
						cerr << "TYPE 1-1 Error!\n";
						printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));
					}
				}
				else if(subinput==0) {	//back to selection menu
					break;
				}
			}
		}
		else if (input == 2) {	//2 input: �۳⿡ ���� ���� ���� ���� ������ �� ������ ����.
			cout << "---- TYPE 2 ----\n\n";
			//with���� ����� �۳� ���� ������ ���� �ް�, �׷κ��� max���� �򵵷� sql�� �ۼ�
			buf = "with max_buy_price as (select customer_id, customer_name, date_year, sum(price * num_of_order) as pricesum from(_order natural join customer) inner join product using(product_id)";
			buf = buf + "where date_year = " + pastyear + " group by customer_id) select customer_id, customer_name, pricesum from max_buy_price where pricesum = (select max(pricesum) from max_buy_price);";
			state = mysql_query(connection, buf.c_str());
			if (state) {
				cerr << "TYPE 2 Error!\n";
				printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));
				continue;
			}
			cout << "customer_id | customer_name | pricesum\n";
			cout << "-------------------------------------\n";
			sql_result = mysql_store_result(connection);
			sql_row = mysql_fetch_row(sql_result);
			cout << sql_row[0] << " | " << sql_row[1] << " | " << sql_row[2];
			string bestmanid = sql_row[0];	//subquery���� ����� vip���� id���� ���� ����
			mysql_free_result(sql_result);

			int subinput;
			while (1) {
				cout << "\n\n----------Subtypes in TYPE 2 ----------\n\n";
				cout << "\t1. TYPE 2-1\n\t0. Back to Select menu\n\n";
				cout << "USER INPUT: ";
				cin >> subinput;
				if (subinput == 1) {
					cout << "-----TYPE 2-1 -----\n\n";
					//�ش� ���� ���� ���� ���� ���� �� ��ǰ�� with���� ����� �ι��� ���� ��´�.
					buf = "with max_buy_unit as(select product_id, product_name, _type, price, manufacturer, date_year, sum(num_of_order) as sum_order from(_order natural join customer) inner join product using(product_id)";
					buf = buf + " where customer_id = " + bestmanid + " and date_year = " + pastyear + " group by product_id) select * from max_buy_unit where sum_order = (select max(sum_order) from max_buy_unit); ";
					cout << "product_id | product_name | type | price | manufacturer | date_year | num_of_order\n";
					cout << "---------------------------------------------------------------------------------\n";
					state = mysql_query(connection, buf.c_str());
					sql_result = mysql_store_result(connection);
					query_print(sql_result);
				}
				else if (subinput == 0)
					break;
			}
		}
		else if (input == 3) {
		//�ֹ� ����, �� ����, ��ǰ ������ join�Ͽ� �۳⿡ �ֹ��� ����� �ִ� �ֹ� ������ view�� �����Ѵ�.
		buf = "create view pastyearsell as select * from _order natural join customer natural join product where date_year = " + pastyear + ";";
			state = mysql_query(connection, buf.c_str());
			if (state) {
				cerr << "View generating error before TYPE3!\n";
				printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));
				continue;
			}
			cout << "---- TYPE 3 ----\n\n";
			//�ȸ� �ݾ��� sum�� �� product_id���� ���� �ش��ϴ� �Ӽ����� ����Ѵ�.
			buf = "select product_id, product_name, _type, price, manufacturer,sum(num_of_order*price) as sold_total from pastyearsell group by product_id order by sold_total desc;";
			state = mysql_query(connection, buf.c_str());
			if (state) {
				cerr << "TYPE 3 Error!\n";
				printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));
				continue;
			}
			cout << "product_id | product_name | type | price | manufacturer | sold_total\n";
			cout << "--------------------------------------------------------------------\n";
			sql_result = mysql_store_result(connection);
			query_print(sql_result);

			int subinput;
			while (1) {
				cout << "\n\n----------Subtypes in TYPE 3 ----------\n\n";
				cout << "\t1. TYPE 3-1\n\t2. TYPE 3-2\n\t0. Back to Select menu\n\n";
				cout << "USER INPUT: ";
				cin >> subinput;
				if (subinput == 1) {	//k�� �Է¹޾� 3�� ������� top k Ʃ���� �̾� ����Ѵ�.
					cout << "-----TYPE 3-1 -----\n\n";
					cout << " Which K ? : ";
					cin >> subinput;
					buf = "select product_id, product_name, _type, price, manufacturer,sum(num_of_order*price) as sold_total from pastyearsell group by product_id order by sold_total desc;";
					state = mysql_query(connection, buf.c_str());
					sql_result = mysql_store_result(connection);
					cout << "product_id | product_name | type | price | manufacturer | sold_total\n";
					cout << "--------------------------------------------------------------------\n";
					query_print(sql_result, subinput);
				}
				else if (subinput == 2) {	//3�� ������� top 10%�� Ʃ���� ����ϱ� ���� row����/10�� ���� �� �׸�ŭ ����Ѵ�.
					cout << "-----TYPE 3-2 -----\n\n";
					buf = "select product_id, product_name, _type, price, manufacturer,sum(num_of_order*price) as sold_total from pastyearsell group by product_id order by sold_total desc;";
					state = mysql_query(connection, buf.c_str());
					sql_result = mysql_store_result(connection);
					int topten = (int)mysql_num_rows(sql_result) / 10;
					cout << "product_id | product_name | type | price | manufacturer | sold_total\n";
					cout << "--------------------------------------------------------------------\n";
					query_print(sql_result, topten);
				}
				else if (subinput == 0)
					break;
			}

			buf = "DROP VIEW pastyearsell";	//view ����
			state = mysql_query(connection, buf.c_str());
		}
		else if (input == 4) {	//3�� ������ ���� �������� �����Ѵ�.
			//3������ ���� �� ����
			buf = "create view pastyearsell as select * from _order natural join customer natural join product where date_year = " + pastyear + ";";
			state = mysql_query(connection, buf.c_str());
			if (state) {
				cerr << "View generating error before TYPE4!\n";
				printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));
				continue;
			}
			cout << "---- TYPE 4 ----\n\n";
			//3���� ���ݰ� ������ ���ߴ� �Ӽ��� �����θ� �ۼ��� ���� ��û
			buf = "select product_id, product_name, _type, price, manufacturer,sum(num_of_order) as sold_total_unit from pastyearsell group by product_id order by sold_total_unit desc;";
			state = mysql_query(connection, buf.c_str());
			if (state) {
				cerr << "TYPE 4 Error!\n";
				printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));
				continue;
			}
			cout << "product_id | product_name | type | price | manufacturer | sold_total_unit\n";
			cout << "-------------------------------------------------------------------------\n";
			sql_result = mysql_store_result(connection);
			query_print(sql_result);

			int subinput;
			while (1) {
				cout << "\n\n----------Subtypes in TYPE 4 ----------\n\n";
				cout << "\t1. TYPE 4-1\n\t2. TYPE 4-2\n\t0. Back to Select menu\n\n";
				cout << "USER INPUT: ";
				cin >> subinput;
				if (subinput == 1) {	//3�� �����ϰ� k �Է¹޾� top k Ʃ�� ���
					cout << "-----TYPE 4-1 -----\n\n";
					cout << " Which K ? : ";
					cin >> subinput;
					buf = "select product_id, product_name, _type, price, manufacturer,sum(num_of_order) as sold_total_unit from pastyearsell group by product_id order by sold_total_unit desc;";
					state = mysql_query(connection, buf.c_str());
					sql_result = mysql_store_result(connection);
					cout << "product_id | product_name | type | price | manufacturer | sold_total_unit\n";
					cout << "-------------------------------------------------------------------------\n";
					query_print(sql_result, subinput);
				}
				else if (subinput == 2) {	//top 10% Ʃ�� ���
					cout << "-----TYPE 4-2 -----\n\n";
					buf = "select product_id, product_name, _type, price, manufacturer,sum(num_of_order) as sold_total_unit from pastyearsell group by product_id order by sold_total_unit desc;";
					state = mysql_query(connection, buf.c_str());
					sql_result = mysql_store_result(connection);
					int topten = (int)mysql_num_rows(sql_result) / 10;
					cout << "product_id | product_name | type | price | manufacturer | sold_total_unit\n";
					cout << "-------------------------------------------------------------------------\n";
					query_print(sql_result, topten);
				}
				else if (subinput == 0)
					break;
			}

			buf = "DROP VIEW pastyearsell";	//�� ����
			state = mysql_query(connection, buf.c_str());
		}
		else if (input == 5) {
			cout << "---- TYPE 5 ----\n\n";
			//product,storestock�� ���Ҿ� offlinestore�� join�� �������� region�� ��� ���� �Ӽ��� �ٷ� �� �ֵ��� ��.
			//California������ ������ �� �� product_id�� ���� �� �������� sum(num_of_storestock)�� 0�̸� ǥ�õǵ��� �ߴ�.
			buf = "select product_id,product_name,_type,manufacturer,region,sum(num_of_storestock) as sum_of_storestock from offlinestore natural join storestock natural join product where region = 'california' group by product_id having sum_of_storestock = 0;";
			state = mysql_query(connection, buf.c_str());
			if (state) {
				cerr << "TYPE 5 Error!\n";
				printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));
				continue;
			}
			cout << "product_id | product_name | type | manufacturer | region | sum_of_storestock\n";
			cout << "----------------------------------------------------------------------------\n";
			sql_result = mysql_store_result(connection);
			query_print(sql_result);
		}
		else if (input == 6) {
			cout << "---- TYPE 6 ----\n\n";
			//got_problem�� set�Ǿ� �ִ�(���� �������� ����) shipping ������ ���
			buf = "select * from shipping where got_problem = 1;";
			state = mysql_query(connection, buf.c_str());
			if (state) {
				cerr << "TYPE 6 Error!\n";
				printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));
				continue;
			}
			cout << "tracking_num | company | address | isarrived | got_problem\n";
			cout << "----------------------------------------------------------\n";
			sql_result = mysql_store_result(connection);
			query_print(sql_result);
		}
		else if (input == 7) {
			//���� �޿� �ȸ� �ֹ� ������ �� �ֹ��� ����� �� ������ �信 ����
			buf = "create view pastmonthsell as select * from _order natural join customer natural join product where date_year = " + year_for_pastmonth + " and date_mon = " + month_for_pastmonth + " ;";
			state = mysql_query(connection, buf.c_str());
			if (state) {
				cerr << "View generating error before TYPE4!\n";
				printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));
				continue;
			}
			cout << "---- TYPE 7 ----\n\n";
			//bill�� contracted customer(account num is not null)�� ���ؼ��� �����ϹǷ�, �ش� ������ �ɾ��ش�.
			buf = "select customer_id,customer_name,order_id,product_name,price,num_of_order,price*num_of_order as total_purchase from pastmonthsell where account_num is not null";
			state = mysql_query(connection, buf.c_str());
			if (state) {
				cerr << "TYPE 7 Error!\n";
				printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));
				continue;
			}
			sql_result = mysql_store_result(connection);
			int n_contracted = (int)mysql_num_rows(sql_result);	//bill�� ���� Ʃ���� ����
			while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
				cout << "-------------------------------------------\n";
				cout << "|\tcustomer_id: " << sql_row[0] << '\n';
				cout << "|\tcustomer_name: " << sql_row[1] << '\n';
				cout << "|\torder_id: " << sql_row[2] << '\n';
				cout << "|\tproduct_name: " << sql_row[3] << '\n';
				cout << "|\tprice: " << sql_row[4] << '\n';
				cout << "|\tnum_of_order: " << sql_row[5] << '\n';
				cout << "|\ttotal_price: " << sql_row[6] << '\n';
				cout << "-------------------------------------------\n\n";
			}
			mysql_free_result(sql_result);

			buf = "DROP VIEW pastmonthsell";	//�� ����
			state = mysql_query(connection, buf.c_str());
			continue;
		}
		else {
			cout << "You have to input values between 0 and 7\n";
		}
	}
	//query routine ends


	fin.open("20181650_delete_drop.txt");	//delete�� drop�� ���� �ִ� �Է����� open
	if (fin.fail()) {
		cerr << "delete&drop txt file open error!\n";
		return 1;
	}
	while (!fin.eof()) {
		getline(fin, buf, ';');
		cout << buf;
		if (buf.empty())
			continue;
		state = mysql_query(connection, buf.c_str());
		if (state)
			printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));
		buf.clear();
	}
	fin.close();
	delete[] temp;
	mysql_close(connection);
	return 0;
}