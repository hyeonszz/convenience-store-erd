# Convenience-Store DB

편의점의 발주·재고·판매 관리 시스템

# 개발환경

OS: Windows 11
Compiler: x86_64-w64-mingw32 (version: 15.1.0)
MySQL Server 8.0
MySQL Server Workbench 8.0

# 컴파일

g++ main.cpp -o main.exe -I"C:\Program Files\MySQL\MySQL Server 8.0\include" -L"C:\Program Files\MySQL\MySQL Server 8.0\lib" -lmysql

동일 폴더(main.exe가 위치한 곳과 동일한 위치)에 libcrypto-3-x64.dll, libmysql.dll, libssl-3-x64.dll 위치시켜 컴파일 진행하였습니다.

# 실행

./main.exe

# 프로그램 사용법

1.  TYPE 1 : 상품 재고 조회
2.  TYPE 2 : 최근 1개월 Top-Selling Items
3.  TYPE 3 : 분기 매출 1위 매장
4.  TYPE 4 : 최대 공급 벤더 + 판매량
5.  TYPE 5 : 재주문 임계 이하 상품
6.  TYPE 6 : VIP 동시구매 Top-3
7.  TYPE 7 : 가맹 vs 직영 품목 다양성
8.  QUIT
