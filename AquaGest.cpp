// AquaGest.cpp : Defines the entry point for the console application.
//
#include <stdio.h>
#include <MainTache.h>

int main(int argc, char* argv[])
{
	CMainTask m_MainTask;

	printf ("main 1\n");
	m_MainTask.Create (1);
	printf ("main 2\n");
	return 0;
}

