<template>
  <div id="app" class="app-container" style="height: 100vh;">
    <header class="app-header">
      <h1>欢迎来到动态问答页面</h1>
      <h2>当前时间: {{ currentTime }}</h2>
    </header>
    <main>
      <input v-model="userInput" placeholder="请输入您的问题..." @keyup.enter="sendQuestion" />
      <button @click="sendQuestion">发送</button>
      <div v-if="response" class="response">
        <h2>响应:</h2>
        <p>{{ response }}</p>
      </div>
      <div v-if="error" class="error">{{ error }}</div>
    </main>
    <footer>
      <p>© 2023 动态问答应用</p>
    </footer>
  </div>
</template>

<script>
import { ref } from 'vue';
import axios from 'axios';

export default {
  setup() {
    const currentTime = ref(new Date().toLocaleString());
    const userInput = ref('');
    const response = ref('');
    const error = ref('');

    setInterval(() => {
      currentTime.value = new Date().toLocaleString();
    }, 1000);

    const sendQuestion = async () => {
      if (!userInput.value.trim()) {
        error.value = '请在提交前输入问题。';
        return;
      }
      error.value = '';
      try {
        const res = await axios.post('http://127.0.0.1:5000/index', {
          question: userInput.value,
        });
        response.value = res.data.response;
      } catch (err) {
        error.value = `请求出错: ${err.message}`;
      }
    };

    return { currentTime, userInput, response, error, sendQuestion };
  },
};
</script>

<style scoped>
html, body {
  height: 100%;
  margin: 0;
}

.app-container {
  display: flex;
  flex-direction: column;
  justify-content: center; /* 垂直居中 */
  align-items: center; /* 水平居中 */
  background: linear-gradient(135deg, #ff7e5f, #feb47b);
  color: white;
  font-family: 'Arial', sans-serif;
  text-align: center;
  padding: 10px;
  border-radius: 10px;
  box-shadow: 0 4px 20px rgba(0, 0, 0, 0.2);
}

.app-header {
  margin-bottom: 20px;
}

input {
  padding: 10px;
  width: 300px;
  border: none;
  border-radius: 5px;
  margin-right: 10px;
  transition: all 0.3s ease;
}

input:focus {
  outline: none;
  box-shadow: 0 0 5px rgba(255, 255, 255, 0.8);
}

button {
  padding: 10px 20px;
  border: none;
  border-radius: 5px;
  background-color: #ff6f61;
  color: white;
  cursor: pointer;
  transition: background-color 0.3s ease;
}

button:hover {
  background-color: #ff4c3b;
}

.response {
  margin-top: 20px;
  animation: fadeIn 0.5s;
  max-height: 200px; /* 设置最大高度 */
  overflow-y: auto; /* 超出部分显示滚动条 */
  padding: 10px; /* 添加内边距 */
  background-color: rgba(255, 255, 255, 0.1); /* 可选：添加背景色以区分 */
  border-radius: 5px; /* 可选：添加圆角 */
}

.error {
  color: #ff4c3b;
  margin-top: 20px;
}

footer {
  margin-top: 10px; /* 减少底部间距 */
  font-size: 0.8em;
}

/* 动画效果 */
@keyframes fadeIn {
  from {
    opacity: 0;
  }
  to {
    opacity: 1;
  }
}
</style>
