from langchain_community.chat_models import ChatZhipuAI
from langchain_core.messages import AIMessage, HumanMessage, SystemMessage
import os
from dotenv import load_dotenv

# 加载环境变量
load_dotenv()

# 从环境变量中获取 API 密钥
api_key = os.getenv("ZHIPUAI_API_KEY")
if api_key is None:
    raise ValueError("API key not found. Please set the ZHIPUAI_API_KEY environment variable.")

chat = ChatZhipuAI(
    model="glm-4",
    temperature=0.5,
)
user_input = input("请输入您的问题：")
messages = [
    AIMessage(content="Hi."),
    SystemMessage(content="Your role is a poet."),
    HumanMessage(content=user_input),
]
response = chat.invoke(messages)
print(response.content)  # Displays the AI-generated poem