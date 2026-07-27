//imports
#define ENV_UTIL_BUFF_LOAD_SIZE 16
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "envutil.h"

//globals
int* m_failedTestNums;
int m_failCapacity;
int m_failCount;

int m_testDoneCount;

//utility function declarations
void push_err(int errNum);
void print_err_arr();
void log_test_finished_message();

#define e(x) int errNum = x;\
  m_testDoneCount++

#define err do {\
  push_err(errNum);\
  return;\
} while(false);

//tests

void load_chunk_0(){
  e(0);
  
  char buffer[ENV_UTIL_MAX_BUFF_SIZE];
  size_t bufferCount = 0;

  FILE *file = fopen("./test1.env", "r");
  assert(file != NULL);

  if(_load_chunk_into_buff(buffer, &bufferCount, file) != 0) err;
  if(strcmp(buffer, "test1abcd\n") != 0) err;
  if(bufferCount != ENV_UTIL_BUFF_LOAD_SIZE - 1) err;
  fclose(file);

  file = fopen("./test2.env", "r");
  assert(file != NULL);

  for(size_t i = 0;i < ENV_UTIL_MAX_BUFF_SIZE;i++) buffer[i] = '\0';
  bufferCount = 0;

  char cmpBuff[64] = "123456789123456789123456789123456789123456789123456789123456789";
  if(ENV_UTIL_BUFF_LOAD_SIZE < sizeof(cmpBuff)) cmpBuff[ENV_UTIL_BUFF_LOAD_SIZE - 1] = '\0';
  if(_load_chunk_into_buff(buffer, &bufferCount, file) != 0) err;
  if(strcmp(buffer, cmpBuff) != 0) err;
  if(bufferCount != ENV_UTIL_BUFF_LOAD_SIZE - 1) err;
  fclose(file);
}

void load_line_1(){
  e(1);

  char buffer[ENV_UTIL_MAX_BUFF_SIZE];
  size_t bufferCount = 0;

  FILE *file = fopen("./test1.env", "r");
  assert(file != NULL);

  if(_load_line_into_buff(buffer, &bufferCount, file) != 0) err;
  if(strcmp(buffer, "test1abcd\n") != 0) err;
  if(bufferCount != 10) err;
  fclose(file);
  
  file = fopen("./test2.env", "r");
  assert(file != NULL);

  for(size_t i = 0;i < ENV_UTIL_MAX_BUFF_SIZE;i++) buffer[i] = '\0';
  bufferCount = 0;

  if(_load_line_into_buff(buffer, &bufferCount, file) != 0) err;
  if(strcmp(buffer, "123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789\n") != 0) err;
  if(bufferCount != 91) err;
  fclose(file);
}

void get_kvp_from_buffer_2(){
  e(2);
  
  char buffer[] = "data1 \n= data2";
  char key[ENV_UTIL_KEY_BUFF_SIZE], value[ENV_UTIL_VALUE_BUFF_SIZE];
  if(_get_kvp_from_buff(buffer, sizeof(buffer), key, value) != 1) err;

  char buffer2[] = "= data2";
  if(_get_kvp_from_buff(buffer2, sizeof(buffer2), key, value) != 2) err;

  char buffer3[] = "data1 da2";
  if(_get_kvp_from_buff(buffer3, sizeof(buffer3), key, value) != 4) err;

  char buffer4[] = "data1 =\n";
  if(_get_kvp_from_buff(buffer4, sizeof(buffer4), key, value) != 5) err;

  char buffer5[] = "data1 = data2\n";
  if(_get_kvp_from_buff(buffer5, sizeof(buffer5), key, value) != 0) err;
  if(strcmp(key, "data1") != 0) err;
  if(strcmp(value, "data2") != 0) err;

  char buffer6[] = "data1 = \"data2\n";
  if(_get_kvp_from_buff(buffer6, sizeof(buffer6), key, value) != 7) err;

  char buffer7[] = "data1 = \"data2\"\n";
  if(_get_kvp_from_buff(buffer7, sizeof(buffer6), key, value) != 0) err;
  if(strcmp(key, "data1") != 0) err;
  if(strcmp(value, "data2") != 0) err;

  char buffer8[] = "data1 = \"dat\\a2\"\n";
  if(_get_kvp_from_buff(buffer8, sizeof(buffer8), key, value) != 8) err;

  char buffer9[] = "data1 = \"data2\'\"\n";
  if(_get_kvp_from_buff(buffer9, sizeof(buffer9), key, value) != 9) err;

  char buffer10[] = "data1 = \"data2\\\'\"\n";
  if(_get_kvp_from_buff(buffer10, sizeof(buffer10), key, value) != 0) err;
  if(strcmp(key, "data1") != 0) err;
  if(strcmp(value, "data2\'") != 0) err;
}

void env_load_3(){
  e(3);
  
  if(env_load("./test3.env", true) != ENV_LOAD_OK) err;

  char *value = getenv("f1");
  if(value == NULL) err;
  if(strcmp(value, "abcd") != 0) err;

  value = getenv("fiel2");
  if(value == NULL) err;
  if(strcmp(value, "1234") != 0) err;

  value = getenv("fld3");
  if(value == NULL) err;
  if(strcmp(value, "123456789123456789") != 0) err;
}

void env_lookup_4(){
  e(4);

  char val[64];
  int result = (int)env_lookup(NULL, "fiel2", val, sizeof(val));
  if(result != (int)ENV_LOOK_ERR_KEY_NOT_FOUND) err;

  result = (int)env_lookup("./test3.env", "fiel2", val, sizeof(val));
  if(result != (int)ENV_LOOK_OK_FILE) err;
  if(strcmp(val, "1234") != 0) err;

  char val2[2];
  result = (int)env_lookup("./test3.env", "fiel2", val2, sizeof(val2));
  if(result != (int)ENV_LOOK_ERR_VALUE_EXCEED_CAPAC) err;

  char val3[4];
  result = (int)env_lookup("./test3.env", "fiel2", val3, sizeof(val3));
  if(result != (int)ENV_LOOK_ERR_VALUE_EXCEED_CAPAC) err;

  char val4[5];
  result = (int)env_lookup("./test3.env", "fiel2", val4, sizeof(val4));
  if(result != (int)ENV_LOOK_OK_FILE) err;
  if(strcmp(val, "1234") != 0) err;

  setenv("test1", "testvalue", true);

  char val5[9];
  result = (int)env_lookup(NULL, "test1", val5, sizeof(val5));
  if(result != (int)ENV_LOOK_ERR_VALUE_EXCEED_CAPAC) err;

  char val6[10];
  result = (int)env_lookup(NULL, "test1", val6, sizeof(val6));
  if(result != (int)ENV_LOOK_OK_ENV) err;
  if(strcmp(val6, "testvalue") != 0) err;

  unsetenv("test1");
}

void parse_comments_5(){
  e(5);
  
  char buf[] = "   data1#234";
  char val[64];
  int result = _parse_value(buf, sizeof(buf), val, sizeof(val), 0);
  if(result != 0) err;
  if(strcmp(val, "data1") != 0) err;

  char buf2[] = "  \"data1#234\"";
  result = _parse_value(buf2, sizeof(buf2), val, sizeof(val), 0);
  if(result != 0) err;
  if(strcmp(val, "data1#234") != 0) err;

}

//main
int main(void){
  m_failedTestNums = malloc(sizeof(int) * 8);
  m_failCapacity = 8;
  m_failCount = 0;
  
  load_chunk_0();
  load_line_1();
  get_kvp_from_buffer_2();
  env_lookup_4();
  env_load_3();
  parse_comments_5();

  log_test_finished_message();
  return 0;
}

void push_err(int errNum){
  if(m_failCapacity == m_failCount){
    m_failCapacity *= 2;
    m_failedTestNums = realloc(m_failedTestNums, sizeof(int) * m_failCapacity);
    assert(m_failedTestNums != NULL);
  }
  m_failedTestNums[m_failCount++] = errNum;
}

void print_err_arr(){
  printf("%d", m_failedTestNums[0]);
  for(int i = 1;i < m_failCount;i++){
    printf(", ");
    printf("%d", m_failedTestNums[i]);
  }
}

void log_test_finished_message(){
  printf("Tests passed : (%d / %d)\n", m_testDoneCount - m_failCount, m_testDoneCount);
  if(m_failCount == 0){
    printf("All tests passed successfully!\n");
  }
  else{
    printf("%d test were not passed :(\n", m_failCount);
    printf("The numbers of the failed tests are : [");
    print_err_arr();
    printf("]\n");
  }
}
