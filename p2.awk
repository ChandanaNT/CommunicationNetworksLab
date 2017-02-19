BEGIN {

	tcp=0;
	udp=0;
	ack=0;
}
{
   event=$5;
   if(event=="cbr")
   {
       udp++;
   }
   if(event=="tcp" )
   {
       tcp++;
   }
   else
   {
       ack++;
   }
	
}
END {
	printf("\n Number of packets sent by TCP : %d",tcp);
	printf("\n Number of packets sent by UDP : %d",udp);
	printf("\n Number of Acknowledgements    : %d \n ",ack);
}