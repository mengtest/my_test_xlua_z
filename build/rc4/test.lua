local rc4 = require 'rc4'

local key = 'MIGJAoGBAKv4OKlpY2oq9QZPMzAjbQfiqDqTnisSvdLP+mTswZJdbtk1J+4+qAySJuZjSQljzcUu0ANg+QG0VsvoU72zu5pErZKWubfe9HB/tq69bhP60qgP6/W2VebWlqUNGtsMedxuVaFBL3SoqU7e5RELIsuArCJJIgz86BQDX0x63VpXAgMBAAE=';

print('crc16 = ',rc4.crc16('this is a test'))

local value = 'this is a test'

local encode_value,encode_len = rc4.encrypt(value,key)
print('encode value :',encode_value,encode_len)
local decode_value,decode_len = rc4.decrypt(encode_value,key)
print('decode value :',decode_value,decode_len)