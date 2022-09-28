#include<stdio.h>
#include<stdlib.h>
#include<libvirt/libvirt.h>
#include<math.h>
#include<string.h>
#include<unistd.h>
#include<limits.h>
#include<signal.h>
#include <time.h>
#include <stdbool.h>
#define MIN(a,b) ((a)<(b)?a:b)
#define MAX(a,b) ((a)>(b)?a:b)

int is_exit = 0; // DO NOT MODIFY THIS VARIABLE

void CPUScheduler(virConnectPtr conn,int interval);

int t=0, num_pcpus=0, num_vms=0, nparams=0;
virDomainPtr *domains = NULL;
float* pcpu_usage, *domain_total_usage, *domain_usage_t1, *domain_usage_t2;
int* vcpu_to_pcpu_mapping;
virTypedParameterPtr params1, params2;

/*
DO NOT CHANGE THE FOLLOWING FUNCTION
*/
void signal_callback_handler()
{
	printf("Caught Signal");
	is_exit = 1;
}

/*
DO NOT CHANGE THE FOLLOWING FUNCTION
*/
int main(int argc, char *argv[])
{
	virConnectPtr conn;

	if(argc != 2)
	{
		printf("Incorrect number of arguments\n");
		return 0;
	}

	// Gets the interval passes as a command line argument and sets it as the STATS_PERIOD for collection of balloon memory statistics of the domains
	int interval = atoi(argv[1]);
	
	conn = virConnectOpen("qemu:///system");
	if(conn == NULL)
	{
		fprintf(stderr, "Failed to open connection\n");
		return 1;
	}

	// Get the total number of pCpus in the host
	signal(SIGINT, signal_callback_handler);

	while(!is_exit)
	// Run the CpuScheduler function that checks the CPU Usage and sets the pin at an interval of "interval" seconds
	{
		CPUScheduler(conn, interval);
		sleep(interval);
	}

	// Closing the connection
	virConnectClose(conn);
	return 0;
}


void reschedule(virDomainPtr* domains, float* domain_total_usage, float* pcpu_usage, int num_vms, int num_pcpus, int* vcpu_to_pcpu_mapping){
	
		float max_pcpu_util = pcpu_usage[0], min_pcpu_util=pcpu_usage[0];
		int highest_pcpu = 0, lowest_pcpu=0;

		for(int i=1; i<num_pcpus; i++){
			if(pcpu_usage[i]>max_pcpu_util)
			{
				highest_pcpu=i;
				max_pcpu_util=pcpu_usage[i];
			}
			if(pcpu_usage[i] < min_pcpu_util ){
				lowest_pcpu=i;
				min_pcpu_util=pcpu_usage[i];
			}
		}
		printf("max pcpu: %f, min pcpu: %f\n", max_pcpu_util, min_pcpu_util);

		if((max_pcpu_util - min_pcpu_util) > 5.0){
			int* domain_index = calloc(num_vms, sizeof(int));
			float* domain_total_usage_temp = calloc(num_vms, sizeof(float));
			for(int i=0; i<num_vms; i++)
			{
					domain_total_usage_temp[i] = domain_total_usage[i];
					domain_index[i]=i;
			}

			for (int i = 0; i < num_vms; ++i){
			for (int j = i + 1; j < num_vms; ++j){
				if (domain_total_usage[i] < domain_total_usage[j]){
					float temp = domain_total_usage[i];
					float temp_index = domain_index[i];
					domain_total_usage[i] = domain_total_usage[j];
					domain_index[i] = domain_index[j];
					domain_total_usage[j] = temp;
					domain_index[j] = temp_index;
				}
			}
		}

			for(int i=0; i<num_vms; i++){
				unsigned char cpumaps = 0x1 << (i%num_pcpus);
				if(virDomainPinVcpu(domains[domain_index[i]], 0, &cpumaps, (num_pcpus/num_vms)+1)){
					printf("error scheduling");
				}

			}
		}

}

/* COMPLETE THE IMPLEMENTATION */
void CPUScheduler(virConnectPtr conn, int interval)
{

	if(t==0){
		t=1;
		virNodeInfoPtr hypervisor_info = malloc(sizeof(virNodeInfo));
		if (virNodeGetInfo(conn, hypervisor_info) != 0){
			return;
		}		
		num_pcpus = hypervisor_info->cpus;		
		num_vms = virConnectListAllDomains(conn, &domains, 0);	
		
		pcpu_usage = calloc(num_pcpus, sizeof(float));
		domain_total_usage = calloc(num_vms, sizeof(float));	
		vcpu_to_pcpu_mapping = calloc(num_vms, sizeof(int));
		domain_usage_t1 = calloc(num_vms, sizeof(float));

		for (int i = 0; i < num_vms; ++i) {
			virDomainPtr dom = domains[i];
			nparams = virDomainGetCPUStats(dom, NULL, 0, -1, 1, 0);
			params1 = calloc(nparams, sizeof(virTypedParameter));
			virDomainGetCPUStats(dom, params1, nparams, -1, 1, 0);
			for(int j=0; j < nparams; j++)
			{
				if(strcmp(params1[j].field, "cpu_time") == 0)
					domain_usage_t1[i] = params1[j].value.ul;
			}
		}
	}
	else{
		t=0;
		for(int i=0; i<num_vms; i++){
			virDomainPtr dom = domains[i];
			domain_usage_t2 = calloc(num_vms, sizeof(float));
			params2 = calloc(nparams, sizeof(virTypedParameter));
			virDomainGetCPUStats(dom, params2, nparams, -1, 1, 0);

			for(int j=0; j<nparams; j++)
			{
				if(strcmp(params2[j].field, "cpu_time")==0){
					domain_usage_t2[i] = params2[j].value.ul;
				}
			}
		
			virVcpuInfoPtr domain_cpu_info = calloc(num_pcpus, sizeof(virVcpuInfo));
			int maplen = VIR_CPU_MAPLEN(num_pcpus);
			unsigned char* cpumaps = calloc(num_pcpus, maplen);
			virDomainGetVcpus(dom, domain_cpu_info, num_pcpus, cpumaps, maplen);
			vcpu_to_pcpu_mapping[i] = domain_cpu_info->cpu;

			float total_time = interval*1.0e9;
			float cpu_time = (float)(domain_usage_t2[i] - domain_usage_t1[i]);
			float percent = cpu_time*100/total_time;
			domain_total_usage[i] = percent;
			pcpu_usage[domain_cpu_info->cpu]+=percent;			
			free(domain_cpu_info);
			free(cpumaps);
		}		
		reschedule(domains,domain_total_usage, pcpu_usage,num_vms,num_pcpus,vcpu_to_pcpu_mapping);
		free(domains);
		free(domain_total_usage);
		free(pcpu_usage);
		free(vcpu_to_pcpu_mapping);
		free(params2);
		free(params1);

	}
}


