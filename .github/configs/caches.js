module.exports = {
  
  openssl: 
  {
    path: [`${process.env.HOME}/.cache/openssl`],
    hashFiles: ['openssl.txt'],
    keyPrefix: 'openssl-',
    restoreKeys: 'openssl-',
  },
  
  proto: 
  {
    path: [`${process.env.HOME}/.cache/proto`],
    hashFiles: ['proto.txt'],
    keyPrefix: 'proto-',
    restoreKeys: 'proto-',
  },
  
  grpc_base: 
  {
    path: [`${process.env.HOME}/.cache/grpc_base`],
    hashFiles: ['grpc_base.txt'],
    keyPrefix: 'grpc_base-',
    restoreKeys: 'grpc_base-',
  },
}
