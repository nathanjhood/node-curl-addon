// This small codeblock in your root-level index.js allows others to consume
// your addon as any other NodeJS module
const node_curl_addon = require(`./lib/addon.node`);
module.exports = node_curl_addon;
