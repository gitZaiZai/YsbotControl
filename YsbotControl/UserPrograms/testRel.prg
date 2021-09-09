dout1:Set( true ) 
dout0:Set( true )
PTP( ap012, Sys_DYN, Sys_OVL )
PTP( ap13, Sys_DYN, Sys_OVL )
PTP( cp1, Sys_DYN, Sys_OVL )
Lin( Sys_Home1, Sys_DYN, Sys_OVL )
PTPRel( Sys_Home1, Sys_DYN, Sys_OVL )
PTP( Sys_Home1, Sys_DYN, Sys_OVL )
PTP( Sys_Home1, Sys_DYN, Sys_OVL )
LinRel( Sys_Home1, Sys_DYN, Sys_OVL )
PTPRel( Sys_Home1, Sys_DYN, Sys_OVL )
dout0:Set( true )
dout1:Set( true ) 
PTP( ap012, Sys_DYN, Sys_OVL )
PTP( ap13, Sys_DYN, Sys_OVL )


--PTP( cp21(), dyn0, Sys_OVL )
--PTP( cp21(), dyn0, Sys_OVL )
dout0:Set( false )
dout1:Set( false )

 










