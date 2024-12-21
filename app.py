from flask import Flask, request, jsonify, send_file
from langchain_community.chat_models import ChatZhipuAI
from langchain_core.messages import AIMessage, HumanMessage, SystemMessage
import os
from flask_cors import CORS
from dotenv import load_dotenv

# 加载环境变量
load_dotenv()

app = Flask(__name__)
CORS(app)  # 启用 CORS

# 从环境变量中获取 API 密钥
api_key = os.getenv("ZHIPUAI_API_KEY")
if api_key is None:
    raise ValueError("API key not found. Please set the ZHIPUAI_API_KEY environment variable.")

chat = ChatZhipuAI(
    model="glm-4",
    temperature=0.5,
)

@app.route('/')
def home():
    return 'Flask is running!'

@app.route('/index', methods=['GET', 'POST'])
def chat_api():
    print("Request Headers:", request.headers)  # Log request headers
    print("Request Data:", request.data)  # Log request body

    # Extract question from different request types
    if request.is_json:
        user_input = request.json
        question = user_input.get('question')
    else:
        try:
            user_input = request.data.decode('utf-8')
            question = user_input
        except Exception as e:
            return jsonify({'error': 'Invalid request data'}), 400

    # Validate question
    if not question:
        return jsonify({'error': 'Question is required'}), 400

    try:
        # Prepare messages for the AI
        messages = [
            SystemMessage(content="You are a helpful AI assistant."),
            HumanMessage(content=question)
        ]

        # Generate response
        response = chat.invoke(messages)
        response_content = response.content

        return jsonify({'response': response_content})

    except Exception as e:
        print(f"Error processing request: {e}")
        return jsonify({'error': 'Internal server error'}), 500

@app.route('/chat')
def chat_interface():
    return send_file('chat.html')  # Return the new chat.html file

if __name__ == '__main__':
    app.run(debug=True)