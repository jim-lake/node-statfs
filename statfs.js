'use strict'

const addon = require('./build/Release/statfs')

exports.statfs = statfs;

function statfs(path) {
  if (typeof path !== 'string') {
    throw new TypeError('`path` must be a string')
  }
  return addon.statfs(path);
}
