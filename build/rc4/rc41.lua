local string = string
local math = math
local table = table

local _M = {}

-- Permutation
_M.permut = function(key)
    local i,j = 0,0
    local cle = {}
    local tempo = 0
    local long = string.len(key)
    local S = {}
    for i = 0, long - 1 do
        cle[i] = string.byte(string.sub(key,i+1,i+1))
    end
    for i = 0, 255 do
        S[i] = i
    end
    for i = 0,255 do
        j = (j+ S[i] + cle[i % long]) % 256
        tempo = S[i]
        S[i] = S[j]
        S[j] = tempo
    end
    return S
end

-- Génération du flot
_M.code =function(texte,S)
    return _M.code_raw(texte,S)
end

_M.code_raw = function(texte,S)
    local i,j = 0,0
    local text = {}
    local maxcara = string.len(texte)
    local code = {}
    local tempo = 0
    local o_chif = 0
    local tResult = {} --changed string concatenation to table concat, much moe efficient
    local cpt = 1
    local S = _M.permut(S)
    for i= 1, maxcara do
        text[i] = string.byte(string.sub(texte,i,i))
    end
    while cpt<maxcara+1 do
        i = (i+1)%256
        j = (j+S[i])%256
        tempo = S[i]
        S[i] = S[j]
        S[j] = tempo
        o_chif = S[(S[i]+S[j])%256]
        tResult[cpt] = string.char(_M.XOR(o_chif,text[cpt]))
        cpt = cpt+1
    end
    return table.concat(tResult)
end

-- Octet1 XOR Octet2
_M.XOR = function(octet1, octet2)
    local O1 = {}
    local O2 = {}
    local result = {}
    local i
    O1 = _M.binaire(octet1)
    O2 = _M.binaire(octet2)
    for i= 1,8 do
        if(O1[i] == O2[i]) then
            result[i] = 0
        else
            result[i] = 1
        end
    end
    return _M.dec(result)
end

-- Transformation binaire
_M.binaire = function(octet)
    local B = {}
    local div = 128
    local i = 1
    while (i < 9) do
        B[i] = math.floor(octet/div)
        if B[i] == 1 then octet = octet-div end
        div = div / 2
        i = i +1
    end
    return B
end

-- Transformation décimale
_M.dec = function(binaire)
    local i
    local result = 0
    local mul = 1
    for i = 8,1,-1 do
        result = result + (binaire[i]* mul)
        mul = mul *2
    end
    return result
end
-- return _M

function _M.randomString(len) 
    len = len or 32
    local chars = {
        "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k",
        "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v",
        "w", "x", "y", "z", "A", "B", "C", "D", "E", "F", "G",
        "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R",
        "S", "T", "U", "V", "W", "X", "Y", "Z", "0", "1", "2",
        "3", "4", "5", "6", "7", "8", "9"
    }
    
    local random_strs = {}
    local index = len
    while index > 0 do
        local char = chars[math.random(1,#chars)]

        table.insert(random_strs,char)
        
        index = index - 1
    end
    return table.concat( random_strs)
end

math.mod = function(a,b)
    return a % b
end
function _M.base64_encode(source_str)
    local b64chars = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/'
    local s64 = ''
    local str = source_str

    while #str > 0 do
        local bytes_num = 0
        local buf = 0

        for byte_cnt=1,3 do
            buf = (buf * 256)
            if #str > 0 then
                buf = buf + string.byte(str, 1, 1)
                str = string.sub(str, 2)
                bytes_num = bytes_num + 1
            end
        end

        for group_cnt=1,(bytes_num+1) do
            b64char = math.fmod(math.floor(buf/262144), 64) + 1
            s64 = s64 .. string.sub(b64chars, b64char, b64char)
            buf = buf * 64
        end

        for fill_cnt=1,(3-bytes_num) do
            s64 = s64 .. '='
        end
    end

    return s64
end
function _M.base64_decode(str64)
    local b64chars = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/'
    local temp={}
    for i=1,64 do
        temp[string.sub(b64chars,i,i)] = i
    end
    temp['=']=0
    local str=""
    for i=1,#str64,4 do
        if i>#str64 then
            break
        end
        local data = 0
        local str_count=0
        for j=0,3 do
            local str1=string.sub(str64,i+j,i+j)
            if not temp[str1] then
                return
            end
            if temp[str1] < 1 then
                data = data * 64
            else
                data = data * 64 + temp[str1]-1
                str_count = str_count + 1
            end
        end
        for j=16,0,-8 do
            if str_count > 0 then
                str=str..string.char(math.floor(data/math.pow(2,j)))
                data=math.mod(data,math.pow(2,j))
                str_count = str_count - 1
            end
        end
    end
    return str
end

function _M.test()
    local key = "Key"
    local text = "\"{\"a\":\"大家好\"}\""
    print(text)
    local K = _M.code(key, text)
    -- 加密后的数据需要base64_encode,此处省略掉
    print(K)
    K = _M.base64_encode(K)
    print(K)
    -- key = 'A3aSeSf2+bo503b3k0YKeU0PSSt7GXJgfzTPoU0H7iMkz1NmRag5AvszoCEKQ9Tnf1k2cNtQ/qT2/nU6CxbenK7OJxLj5Fjy1f78Y5uttudQPhkUGesjJD7+3h1qKHTH'
    -- K = 'P3HCszJlDsO+JsOqw7PCvMOEw7PDnzYywrXCqWfDmcKrw7o3wrzCusKj';
    K = _M.base64_decode(K)
    print(K)
    
    -- 解密之前先要base64_decode,此处省略掉
    text = _M.code(key, K)
    print(text)

    -- math.randomseed(tostring(os.time()):reverse():sub(1, 6))
    -- for i=0,10 do
    --     print(_M.randomString(32))
    -- end

    -- print(_M.randomString(128))
end

_M.test()

return _M
