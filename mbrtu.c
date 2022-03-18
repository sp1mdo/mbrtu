#include <stdio.h>
#include <modbus.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

int main(int argc, char *argv[])
{

int scan=0;

if(argc<4) 
	{	
	printf("too few arguments!\n usage %s portname slave_id baud_rate function_code\n",argv[0]);
	return 1;
	} 



char portname[20];
uint16_t *registers;
uint8_t *inputs;
modbus_t *ctx;
int slave_id = atoi(argv[2]);
int baudrate = atoi(argv[3]);
int fc=atoi(argv[4]);
int addr=atoi(argv[5])-1;
int nb=atoi(argv[6]);
int rc,i;

uint32_t old_response_to_sec;
uint32_t old_response_to_usec;

if(fc == 16 && argc-7 != atoi(argv[6]))
{
	printf("register number, and number of parameters do not match!\n");
	return 1;
}

//printf("slave_id=%d BAUD=%d start_reg=%d, reg_count=%d\n",slave_id,baudrate,addr,nb);

registers = (uint16_t *) calloc(nb+1,sizeof(uint16_t));
inputs = (uint8_t *) calloc(nb+1,sizeof(uint8_t));

strcpy(portname,argv[1]);
ctx = modbus_new_rtu(portname, baudrate, 'N', 8, 1);
modbus_set_slave(ctx, slave_id);

if (modbus_connect(ctx) == -1)
	{
	fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
	modbus_free(ctx);
  	return -1;
	}

modbus_get_response_timeout(ctx, &old_response_to_sec, &old_response_to_usec);

/* Define a new and too short timeout! */
//printf("timeout %d %d---- \n", old_response_timeout.tv_sec, old_response_timeout.tv_usec);

uint32_t response_timeout_sec = 2;
uint32_t response_timeout_usec = 0;

modbus_set_response_timeout(ctx, response_timeout_sec , response_timeout_usec);



if(fc==2) //read discreete inputs
{
    rc=modbus_read_input_bits(ctx, addr, nb, inputs);
    if(rc == -1)
    {
	fprintf(stderr, "Read discreete inputs, %s\n", modbus_strerror(errno));
	return -1;
    }
}




if(fc==3) //read holding registers
{
    rc=modbus_read_registers(ctx,addr,nb,registers);
    if(rc == -1)
    {
	fprintf(stderr, "Read holding registers, %s\n", modbus_strerror(errno));
	return -1;
    }
}


if(fc==4) //read input registers
{
	rc=modbus_read_input_registers(ctx,addr,nb,registers);
	if(rc == -1)
	{
	    fprintf(stderr, "Read input registers, %s\n", modbus_strerror(errno));
	    return -1;
	}
}

if(fc==5) //write single coil
{
	//printf("write single coil\n");

	rc=modbus_write_bit(ctx, addr, nb);
if(rc == -1)
	{
	fprintf(stderr, "Write single coil, %s\n", modbus_strerror(errno));
	return -1;
	}
}


if(fc==6) //write single register
{
	rc=modbus_write_register(ctx,addr,nb);
	if(rc == -1)
	{
		fprintf(stderr, "Write single registers, %s\n", modbus_strerror(errno));
		return -1;
	}
}

if(fc==16) //write multiple registers
{
	for(i=0;i<argc-7;i++)
	{
		registers[i]=atoi(argv[i+7]);
		printf("reg[%d]=%d\n",i,registers[i]);

	}
	rc=modbus_write_registers(ctx,addr,nb,registers);
	if(rc == -1)
	{
	fprintf(stderr, "Write multiple registers, %s\n",modbus_strerror(errno));
	return -1;
	}
printf("argc=%d %s\n",argc,argv[5]);
}


if(fc == 2)
{
for(i = 0; i<nb;i++)
    printf("%d ",inputs[i]);
printf("\n");
}

if(fc==3 || fc==4)
for(i=0;i<nb;i++)
{
    if(registers[i] != 0 ) printf("register[%d] = %u\n",i+addr,registers[i]);
}

modbus_close(ctx);
modbus_free(ctx);
free(registers);
free(inputs);

return 0;
}
