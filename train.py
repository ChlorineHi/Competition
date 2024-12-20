import torch
import torch.nn as nn
import torch.optim as optim
from torch.utils.data import DataLoader, Dataset
from model.llm_model import LLM

# 超参数设置
EMBED_SIZE = 256  # 嵌入维度
HEADS = 8  # 注意力头数
NUM_LAYERS = 6  # Transformer 层数
FORWARD_EXPANSION = 4  # 前馈层扩展
DROPOUT = 0.1  # Dropout 概率
MAX_LENGTH = 100  # 最大序列长度
VOCAB_SIZE = 10000  # 词汇表大小
LEARNING_RATE = 0.001  # 学习率
EPOCHS = 10  # 训练轮数

# 模型初始化
model = LLM(EMBED_SIZE, HEADS, NUM_LAYERS, FORWARD_EXPANSION, DROPOUT, MAX_LENGTH, VOCAB_SIZE)

# 优化器和损失函数
optimizer = optim.Adam(model.parameters(), lr=LEARNING_RATE)
loss_fn = nn.CrossEntropyLoss()

# 数据集和数据加载器
class CustomDataset(Dataset):
    def __init__(self, data):
        self.data = data

    def __len__(self):
        return len(self.data)

    def __getitem__(self, index):
        return self.data[index]

# 假数据示例
train_data = [(torch.randint(0, VOCAB_SIZE, (MAX_LENGTH,)), torch.randint(0, VOCAB_SIZE, (MAX_LENGTH,))) for _ in range(1000)]
train_dataset = CustomDataset(train_data)
train_loader = DataLoader(train_dataset, batch_size=32, shuffle=True)

# 训练循环
for epoch in range(EPOCHS):
    for x, y in train_loader:
        optimizer.zero_grad()
        output = model(x)
        loss = loss_fn(output.view(-1, VOCAB_SIZE), y.view(-1))
        loss.backward()
        optimizer.step()
        print(f'Epoch [{epoch + 1}/{EPOCHS}], Loss: {loss.item():.4f}')
