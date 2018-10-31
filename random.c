#include "random.h"

static time_t t;

void init_random(){
	srand((unsigned)time(&t));
}

bool get_random_bool(int p_true){
	return rand() % 100 < p_true;
}