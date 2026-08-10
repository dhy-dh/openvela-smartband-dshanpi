import storage from '@system.storage';
import { promisify } from './promisify';

const _get = promisify(storage, 'get');
const _set = promisify(storage, 'set');
const _clear = promisify(storage, 'clear');

export default {
  getItem(key) {
    return _get({ key });
  },

  setItem(key, value) {
    return _set({ key, value });
  },

  clear() {
    return _clear();
  }
}