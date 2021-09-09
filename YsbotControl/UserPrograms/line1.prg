PTP(ap1,dyn1)
while (true ) do
	--PTP( Sys_Home1, dyn1 )
	PTP( cp1, dyn1 )
	--dout0:Set( false )
	--dout1:Set( false )
	Lin( cp0, dyn1 )
	--WaitTime( 5000 )
	Lin(cp1,dyn1)
	--dout0:Set( true )
	--dout1:Set( true )
	PTP(ap1,dyn1)
	--din0:Wait( true, 0 )
end