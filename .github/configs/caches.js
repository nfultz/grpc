module.exports = {
  
  openssl: 
  {
    path: [`${process.env.HOME}/.cache/openssl`],
    hashFiles: ['requirements*.txt'],
    keyPrefix: 'openssl-',
    restoreKeys: 'openssl-',
  },
  
}
