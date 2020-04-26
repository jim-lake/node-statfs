
const statfs = require('./statfs.js');

statfs.statfs("/").then(result => {
  console.log("result:",result);
},err => {
  console.log("error:",err);
});
