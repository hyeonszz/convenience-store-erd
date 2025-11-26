#ifndef DATABASE_H
#define DATABASE_H

#include <mysql.h>
#include <string>

// 전역 연결 포인터
extern MYSQL *conn;

// DB 연결 및 종료 함수
bool connectDB();
void closeDB();
void execute(const std::string &query);

// 쿼리 실행 함수 선언 
void ProductAvailabilityQuery(); // 1. 특정 제품 재고 조회
void TopSellingItemsQuery();    // 2. 가장 많이 팔린 상품
void StorePerformanceQuery();   // 3. 가장 매출 높은 매장
void VendorStatisticsQuery();  // 4. 가장 많이 입고된 업체
void InventoryAlertsQuery();  // 5. 재주문 임계치 이하 상품
void CustomerPatternsQuery(); // 6. VIP 고객 Top 3 구매 상품
void FranchiseComparisonQuery(); // 7. 가맹점 vs 직영점 상품 다양성

#endif // DATABASE_H
