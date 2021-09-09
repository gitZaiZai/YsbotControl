package.path=package.path .. ";D:\?.lua"

--return
require ("add")
cnt = 6 
repeat 
res = string.format(" the value is = %d " ,add(3,cnt))
print(res)
cnt = cnt + 1 
until ( cnt > 5  )









