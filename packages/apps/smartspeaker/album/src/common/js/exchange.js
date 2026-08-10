import exchange from '@system.exchange';
import { promisify } from './promisify';

const get = promisify(exchange, 'get');
const set = promisify(exchange, 'set');

export {
  get, set
}

export default {
  get, set
}
