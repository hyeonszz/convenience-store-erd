drop table Sale_Record;
drop table Transaction_Info;
drop table Order_Detail;
drop table Order_Header;
drop table Inventory;
drop table Customer;
drop table Product;
drop table Vendor;
drop table Store;

create table Store
   (store_id   int,
    name      varchar(50) NOT NULL,
    city      varchar(50),
     phone_number   varchar(20),
     ownership_type   ENUM('Franchise','Corporate'),
     open_time TIME,
     close_time TIME,
    primary key (store_id)
   );

create table Vendor
   (vendor_id   int,
     name   varchar(50) NOT NULL,
     contact_info varchar(100),
     primary key (vendor_id)
   );

create table Product
   (product_id   int,
	product_name    varchar(50) NOT NULL,
	brand   varchar(50),
	price   decimal(10,2),
	vendor_id   int,
	primary key (product_id),
	foreign key (vendor_id) references vendor(vendor_id) 
      on delete set null
   );
    
create table Customer
   (customer_id int,
     name varchar(50) NOT NULL,
    phone varchar(20),
     email varchar(50),
     loyalty_grade ENUM('Regular','VIP'),
    primary key (customer_id)
   );

create table Order_Header (
	order_id int,
    store_id int,
    order_date DATE DEFAULT NULL,
    primary key(order_id),
	foreign key(store_id) references Store(store_id)
		on delete cascade
);

create table Order_Detail (
    order_id  int,
    product_id int,
    quantity  int check (quantity >= 0),
    primary key (order_id, product_id),
    foreign key (order_id)  REFERENCES Order_Header(order_id)  ON DELETE CASCADE,
	foreign key (product_id) REFERENCES Product(product_id)    ON DELETE CASCADE
);

create table Transaction_Info
   (transaction_id   int,
    store_id   int,
     customer_id   int,
     transaction_date   date,
     transaction_time   time,
     payment_method ENUM('Cash','Card'),
     total_amount decimal(10,2),
    primary key (transaction_id),
    foreign key (store_id) references Store(store_id)
      on delete cascade,
    foreign key (customer_id) references Customer(customer_id)
      on delete cascade
   );

create table Sale_Record
   (transaction_id   int,
     product_id int,
     quantity int check(quantity>=0),
     unit_price decimal(10,2),
     primary key(transaction_id, product_id),
     foreign key(transaction_id) references Transaction_Info(transaction_id)
      on delete cascade,
    foreign key(product_id) references Product(product_id)
      on delete cascade
   );

create table Inventory
   (store_id int,
    product_id int,
    quantity int check(quantity>=0),
     reorder_threshold int check(reorder_threshold >=0),
     reorder_quantity int check(reorder_quantity>=0),
     primary key(store_id, product_id),
     foreign key(store_id) references Store(store_id)
      on delete cascade,
    foreign key(product_id) references Product(product_id)
      on delete cascade
   );