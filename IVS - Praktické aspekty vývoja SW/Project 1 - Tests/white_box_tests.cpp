//======== Copyright (c) 2021, FIT VUT Brno, All rights reserved. ============//
//
// Purpose:     White Box - Tests suite
//
// $NoKeywords: $ivs_project_1 $white_box_code.cpp
// $Author:     Samuel Valaštín <xvalas10@stud.fit.vutbr.cz>
// $Date:       $2021-01-04
//============================================================================//
/**
 * @file white_box_tests.cpp
 * @author Samuel Valaštín
 * 
 * @brief Implementace testu prace s maticemi.
 */

#include "gtest/gtest.h"
#include "white_box_code.h"

//============================================================================//
// ** ZDE DOPLNTE TESTY **
//
// Zde doplnte testy operaci nad maticemi. Cilem testovani je:
// 1. Dosahnout maximalniho pokryti kodu (white_box_code.cpp) testy.
// 2. Overit spravne chovani operaci nad maticemi v zavislosti na rozmerech 
//    matic.
//============================================================================//
class TestMatrix : public ::testing::Test {
    protected:
    Matrix MatrixA;
    Matrix MatrixB;
    Matrix MatrixC;
};

TEST_F(TestMatrix,classconstruct){

    // try construct 1x1
    MatrixA = Matrix();
    MatrixB = Matrix();
    EXPECT_EQ(MatrixA,MatrixB);

    // try construct 2x2 matrixes
    MatrixA = Matrix(2,2);
    MatrixB = Matrix(2,2);
    EXPECT_EQ(MatrixA,MatrixB);

    // test invalid constructs
    EXPECT_ANY_THROW(Matrix(0,122));
    EXPECT_ANY_THROW(Matrix(122,0));
}

TEST_F(TestMatrix,setandget){

    #define setMatrixRows 4
    #define setMatrixCols 4
    MatrixA = Matrix(setMatrixRows,setMatrixCols);

    // try set value
    EXPECT_TRUE(MatrixA.set(0,0,1.0));
    EXPECT_FALSE(MatrixA.set(7,2,1.0));

    // try set value by another vector
    std::vector<std::vector<double>> setMatrix;
    
    std::vector<double>tmp;
    double setValue = 1.0;
    for(size_t row = 0; row < setMatrixRows;row++){
        for(size_t col = 0; col < setMatrixCols;col++){
            tmp.push_back(setValue);
            setValue = setValue + 1.0;
        }
        setMatrix.push_back(tmp);
        tmp.clear();
    }
    EXPECT_TRUE(MatrixA.set(setMatrix));

    // try set with invalid size
    MatrixB = Matrix(2,3);
    EXPECT_FALSE(MatrixB.set(setMatrix));
    #undef setMatrixRows
    #undef setMatrixCols

    //test get method
    EXPECT_EQ(MatrixB.get(0,0),MatrixB.get(1,1));

    //test get exception
    EXPECT_ANY_THROW(MatrixA.get(14,0));
    EXPECT_ANY_THROW(MatrixA.get(0,14));
}

TEST_F(TestMatrix,operatortest){

    /***************       operator== tests    *****************/
    MatrixA = Matrix(3,2);
    MatrixB = Matrix(3,2);
    EXPECT_TRUE(MatrixA.operator==(MatrixB));
    for(size_t row = 0; row < 3; row++){
        for(size_t col = 0 ; col < 2; col++){
            MatrixB.set(row,col,1.2);
        }
    }
    EXPECT_FALSE(MatrixA.operator==(MatrixB));
    MatrixB = Matrix(2,3);
    EXPECT_ANY_THROW(MatrixA.operator==(MatrixB));

    /***************       operator+ tests    *****************/
    EXPECT_ANY_THROW(MatrixC = MatrixA.operator+(MatrixB));

    MatrixA = Matrix(10,10);
    MatrixB = Matrix(10,10);
    EXPECT_NO_THROW(MatrixC = MatrixA.operator+(MatrixB));

    /***************       operator* tests    *****************/
    EXPECT_NO_THROW(MatrixC = MatrixA.operator*(MatrixB));
    MatrixB = Matrix(12,1);
    EXPECT_ANY_THROW(MatrixC = MatrixA.operator*(MatrixB));

    MatrixA = Matrix(7,7);
    MatrixB = Matrix(7,7);
    for(size_t i = 0 ; i < 7 ; i++){
        for(size_t j = 0; j < 7 ; j++){
            MatrixA.set(i,j,5.0);
            MatrixB.set(i,j,1.0);
        }
    }
    EXPECT_NO_THROW(MatrixC = MatrixA.operator*(1.0));
    ASSERT_EQ(MatrixC.get(0,0),MatrixA.get(0,0));
}

TEST_F(TestMatrix,lineareq){

    MatrixB = Matrix(2,2);
    // 2x1 - x2 = 6
    // 2x1 + 2x2 = 18
    MatrixB.set(0,0,2.0);
    MatrixB.set(0,1,-1.0);
    MatrixB.set(1,0,2.0);
    MatrixB.set(1,1,2.0);
    std::vector<double> B1;
    B1.push_back(6.0);
    B1.push_back(18.0);
    std::vector<double> x1x2;
    x1x2.push_back(5.0);
    x1x2.push_back(4.0);
    std::vector<double>expRes = MatrixB.solveEquation(B1);
    ASSERT_EQ(x1x2[0],expRes[0]);
    ASSERT_EQ(x1x2[1],expRes[1]);

    //try with empty right side - exception
    std::vector<double>empty;
    std::vector<double>expErr;
    EXPECT_ANY_THROW(expErr = MatrixB.solveEquation(empty));

    //try with no sqare matrix - exception
    MatrixC = Matrix(1,4);
    MatrixC.set(0,0,5.0);
    MatrixC.set(0,1,2.0);
    std::vector<double>noSqare;
    noSqare.push_back(15.0);
    noSqare.push_back(0.0);
    noSqare.push_back(0.0);
    noSqare.push_back(0.0);
    std::vector<double>expNoSqare;
    EXPECT_ANY_THROW(expNoSqare = MatrixC.solveEquation(noSqare));

    //singular matrix - exception
    MatrixB.set(0,0,16.0);
    MatrixB.set(0,1,4.0);
    MatrixB.set(1,0,4.0);
    MatrixB.set(1,1,1.0);
    std::vector<double> singularB;
    singularB.push_back(36.0);
    singularB.push_back(9.0);
    std::vector<double>singRes;
    EXPECT_ANY_THROW(singRes = MatrixB.solveEquation(singularB));
}

TEST_F(TestMatrix,advancedeq){
    //advanced test suite math example inspirated by:  https://www.math.sk/skripta/node59.html
    MatrixA = Matrix(3,3);
    MatrixA.set(0,0,1.0);
    MatrixA.set(0,1,2.0);
    MatrixA.set(0,2,1.0);
    MatrixA.set(1,0,3.0);
    MatrixA.set(1,1,2.0);
    MatrixA.set(1,2,1.0);
    MatrixA.set(2,0,4.0);
    MatrixA.set(2,1,3.0);
    MatrixA.set(2,2,-2.0);
    std::vector<double> B2;
    B2.push_back(8.0);
    B2.push_back(10.0);
    B2.push_back(4.0);
    std::vector<double> expectedRes;
    expectedRes.push_back(1.0);
    expectedRes.push_back(2.0);
    expectedRes.push_back(3.0);
    std::vector<double> solveEqRes = MatrixA.solveEquation(B2);
    
    ASSERT_EQ(expectedRes[0],solveEqRes[0]); // 1.0 == 1.0
    ASSERT_EQ(expectedRes[1],solveEqRes[1]); // 2.0 == 2.0
    ASSERT_EQ(expectedRes[2],solveEqRes[2]); // 3.0 == 3.0

    // 1x1 matrix
    MatrixB = Matrix();
    MatrixB.set(0,0,4.0);
    std::vector<double>soloB;
    soloB.push_back(32.0);
    std::vector<double>soloRes;
    soloRes = MatrixB.solveEquation(soloB);
    EXPECT_EQ(soloRes[0],8.0);

    // 4x4 matrix 
    MatrixC = Matrix(4,4);
    std::vector<double>fourfourB;

    // 5x +2y + 3z + w = 24
    MatrixC.set(0,0,5.0);
    MatrixC.set(0,1,2.0);
    MatrixC.set(0,2,3.0);
    MatrixC.set(0,3,1.0);
    fourfourB.push_back(99.0);

    //4x + 2y + 1z + 5w = 5
    MatrixC.set(1,0,4.0);
    MatrixC.set(1,1,2.0);
    MatrixC.set(1,2,1.0);
    MatrixC.set(1,3,5.0);
    fourfourB.push_back(122.0);

    //4x + 5y + z + w = 97
    MatrixC.set(2,0,4.0);
    MatrixC.set(2,1,5.0);
    MatrixC.set(2,2,1.0);
    MatrixC.set(2,3,1.0);
    fourfourB.push_back(97.0);
    
    // x+y+z+w = 40
    MatrixC.set(3,0,1.0);
    MatrixC.set(3,1,1.0);
    MatrixC.set(3,2,1.0);
    MatrixC.set(3,3,1.0);
    fourfourB.push_back(40.0);

    std::vector<double>expRes = MatrixC.solveEquation(fourfourB);
    double acc = 7.0;
    for(size_t i = 0 ; i < 4 ; i++){
        EXPECT_EQ(expRes[i],acc);
        acc = acc + 2.0;
    }
}

TEST_F(TestMatrix,transposetest){
    MatrixA = Matrix(3,3);
    MatrixB = Matrix(3,3);
    std::vector<double> values {5.0,10.0,15.0,20.0,25.0,30.0,35.0,40.0,45.0};
    uint16_t index = 0;

    for(size_t row = 0 ; row < 3 ; row++){
        for(size_t col = 0; col < 3; col++){
            MatrixA.set(row,col,values[index++]);
        }
    }

    MatrixB.set(0,0,values[0]);
    MatrixB.set(0,1,values[3]);
    MatrixB.set(0,2,values[6]);
    MatrixB.set(1,0,values[1]);
    MatrixB.set(1,1,values[4]);
    MatrixB.set(1,2,values[7]);
    MatrixB.set(2,0,values[2]);
    MatrixB.set(2,1,values[5]);
    MatrixB.set(2,2,values[8]);
    MatrixC = Matrix(3,3);

    EXPECT_NO_THROW(MatrixC = MatrixA.transpose());

    for(size_t row = 0 ; row < 3 ; row++){
        for(size_t col = 0; col < 3; col++){

            ASSERT_EQ(MatrixC.get(row,col),MatrixB.get(row,col));
        }
    }

    // reverse transpose
    EXPECT_NO_THROW(MatrixC = MatrixB.transpose());
    for(size_t row = 0 ; row < 3 ; row++){
        for(size_t col = 0; col < 3; col++){
            ASSERT_EQ(MatrixC.get(row,col),MatrixA.get(row,col));
        }
    }
}

TEST_F(TestMatrix,inversetest){

    // simple 2x2 matrixes
    MatrixA = Matrix(2,2);
    MatrixB = Matrix(2,2);
    MatrixC = Matrix(2,2);

    std::vector<double> expectedValues {1.0,-2.0,-2.0,5.0};
    MatrixA.set(0,0,5.0);
    MatrixA.set(0,1,2.0);
    MatrixA.set(1,0,2.0);
    MatrixA.set(1,1,1.0);

    EXPECT_NO_THROW(MatrixC = MatrixA.inverse());
    int index = 0;
    for(size_t row = 0 ; row < 2 ; row++){
        for(size_t col = 0; col < 2; col++){
            ASSERT_EQ(MatrixC.get(row,col),expectedValues[index++]);
        }
    }
    //invalid 2x2 matrix (singular)
    MatrixA.set(0,0,3.0);
    MatrixA.set(0,1,6.0);
    MatrixA.set(1,0,1.0);
    MatrixA.set(1,1,2.0);
    EXPECT_ANY_THROW(MatrixC = MatrixA.inverse());

    // try to set 1x1 inverse
    MatrixA = Matrix();
    MatrixB = Matrix();
    MatrixA.set(0,0,15.45);
    EXPECT_ANY_THROW(MatrixB = MatrixA.inverse());

    // 3x3 matrixes
    MatrixA = Matrix(3,3);
    MatrixB = Matrix(3,3);
    MatrixC = Matrix(3,3);

    MatrixA.set(0,0,4.0);
    MatrixA.set(0,1,2.0);
    MatrixA.set(0,2,1.0);
    MatrixA.set(1,0,1.0);
    MatrixA.set(1,1,1.0);
    MatrixA.set(1,2,2.0);
    MatrixA.set(2,0,0.0);
    MatrixA.set(2,1,0.0);
    MatrixA.set(2,2,1.0);
    std::vector<double> expecRes = {0.5,-1.0,1.5,-0.5,2.0,-3.5,0.0,0.0,1.0};
    EXPECT_NO_THROW(MatrixB = MatrixA.inverse());

    int i = 0;
    for(size_t row = 0 ; row < 3 ; row++){
        for(size_t col = 0; col < 3; col++){
            ASSERT_EQ(MatrixB.get(row,col),expecRes[i++]);
        }
    }
}

TEST_F(TestMatrix,anothersizes){

    #define MAX_LEN 9
    #define MAX_WID 9

    for(size_t x = 1; x < MAX_LEN ; x++){

        for(size_t y = 1 ;y < MAX_WID ; y++){

            // construct matrixes
            MatrixA = Matrix(x,y);
            MatrixB = Matrix(x,y);

            for(size_t x1 = 0 ; x1 < x ; x1++){
                for(size_t y1 = 0 ; y1< y ; y1++){
                    ASSERT_EQ(MatrixA.get(x1,y1),MatrixB.get(x1,y1));

                    if(x1 != 0 && y1 != 0){

                        double setValue = (double)x1 + (double)y1;
                        MatrixA.set(x1,y1,setValue);
                        EXPECT_NE(MatrixA.get(x1,y1),MatrixB.get(x1,y1));
                        MatrixB.set(x1,y1,setValue);
                        EXPECT_EQ(MatrixA.get(x1,y1),MatrixB.get(x1,y1));
                    }
                }
            }
            // check if matrixes are equal for each size
            ASSERT_TRUE(MatrixA.operator==(MatrixB));

            // test operator+ results
            MatrixC = Matrix(x,y);
            MatrixC = MatrixA.operator+(MatrixB);

            for(size_t x1 = 0; x1< x; x1++){

                for(size_t y1 = 0 ; y1 < y ; y1++){

                    EXPECT_EQ(MatrixC.get(x1,y1),MatrixB.get(x1,y1)+MatrixA.get(x1,y1));
                }
            } 

            // condition for * two matrixes is MatrixA.rows == MatrixB.cols
            if(x == y){
              
                MatrixC = Matrix(x,y);
                MatrixC = MatrixA.operator*(MatrixB);
                std::vector<double>rowA;
                std::vector<double>colB;

                for(size_t x1 = 0; x1 < x; x1++){

                    for(size_t y1 = 0; y1 < y; y1++){

                        if(MatrixA.get(x1,y1) == (double)0 || MatrixB.get(x1,y1) == (double)0){
                            ASSERT_EQ(MatrixC.get(x1,y1),0.0);
                        }
                        else {
                            
                            // get the row from first Matrix
                            for(size_t j = 0 ; j < x ; j++){
                                rowA.push_back(MatrixA.get(x1,j));
                            }

                            // get the column from second Matrix
                            for(size_t k = 0 ; k < y ; k++){
                                colB.push_back(MatrixA.get(k,y1));
                            }

                            //calculate expected value
                            double expectedValue = 0.0;
                            for(size_t i = 0; i < rowA.size();i++){
                                expectedValue += rowA[i] * colB[i];
                            }

                            EXPECT_EQ(MatrixC.get(x1,y1),expectedValue);
                            rowA.clear();
                            colB.clear();
                        }
                    }
                }
            }
        }
    }
}
/*** Konec souboru white_box_tests.cpp ***/
