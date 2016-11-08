/* sampleCodeModule.c */

int two();
void interumpitest();
void syscall(int a, int b, int c, int d);


char * v = (char*)0xB8000;

static int var1 = 0;
static int var2 = 0;

static int k = 0;

int main() {
	//All the following code may be removed 



	syscall(0, "Hola que tal?", 0, 0);







	while(1){
		k++;
		
		if (k > 10000000)
		{

			k = 0;
		}
	}



	//Test if BSS is properly set up
	if (var1 == 0 && var2 == 0)
		return 0xDEADC0DE;

	return 0xDEADBEEF;
}