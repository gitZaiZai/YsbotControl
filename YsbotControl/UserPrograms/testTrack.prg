bTrackActive(  true )
Dyn( dyn1 )
Ovl( ove0 )
conve0:BeginTrack(  )
while ( bTrackActive(  ) ) do 
	RefSys( Sys_Base )
	PTP( cp0, dyn1, ove0 )
        conve0:WaitObj( object )
        RefSys( object:Coord( )  )
	PTP( cp5, dyn1, oveAbort )
  	Lin( cp51, dyn1, oveBlend )
	WaitIsFinished( )
  	Lin( cp5, dyn1, ove0 )
	WaitIsFinished( )
	conve0:DoneTrack( object, true )
	RefSys( Sys_Base )
end
conve0:EndTrack(  )

