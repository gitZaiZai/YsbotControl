PTP( leftup, dyn1 )
--RefSys( Sys_Base )
--Tool( Sys_Flange )
while (true ) do
	Lin( leftbom, dyn1 )
	Lin( leftup, dyn1 )
	Lin( rightup, dyn1 )
	WaitTime( 1000 )
	Lin( rightbom, dyn1 )
	Lin( rightup, dyn1 )
	Lin( leftup, dyn1 )
end
