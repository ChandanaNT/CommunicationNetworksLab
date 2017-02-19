BEGIN {
	tcp=0;
	udp=0;
}
{
	event=$1;
	packet=$5;

	if(event=="r")
	{
        if(packet=="tcp")
        {
	    tcp+=1;
	    }
	}
	if(event=="r" )
	{
	    if(packet=="cbr")
	    {
	    udp+=1;
	    }
	}
}
END {
  printf("\n Throughput of TCP is %f ",tcp/123.0);
  printf("\n Throughput if UDP is %f \n",udp/123.0);	

}