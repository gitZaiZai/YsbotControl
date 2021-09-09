--[ lua new program test]--
while(true) do
    local ret = din1:wait(true,8000)
    if( ret == -1) then
          print("ret = ",ret)
          break
    else
         PTP(ap0)
        PTP(ap1)
    end
end
print("program end.\n ")
--dout0:set(true)
--dout1:set(true)
--dout0:set(false)
--dout1:set(false)

