st = "b"
alpha = "abcdefghijklmnopqrstuvwxyz"
ret = 0
inter =0
def check(st):
    boo = False
    for i in range(len(st)-1):
        if(st[i] < st[i+1] and not boo):
            boo = True
        elif(st[i] < st[i+1] and boo):
            return False
    return boo


for k in alpha:
    st = k
    inter = 0
    iter1 = 0
    iter2 = 0
    for i in alpha:
        if i in st:
            continue
        st = st + i
        for j in alpha:
            if j in st:
                continue
            st = st + j
            if check(st):
                if(st[0] < st[1]):
                    iter1 += 1
                else:
                    iter2 += 1        
                inter += 1
            st = st[:2]
        st = st[:1]
    ret += inter
    print(f"{k}: {inter} = {iter1} + {iter2}")

print(ret)
