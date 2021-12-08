# Torrente 


## Contexto e Motivação

O Torrente surge da necessidade de uma ferramenta que atenda os requisitos de segurança necessários para a iniciativa do Amazon Biobank, um banco de dados de biodiversidade da amazônia em blockchain¹.
Buscando alcançar disponibilidade, integridade e confiabilidade em um contexto de transmissão de arquivos distribuído, o projeto parte do protocolo BitTorrent como seu ponto de partida. Entretanto, aspectos de incentivo se fazem necessários para viabilizar o sistema como uma ferramenta atrativa para colaboradores, pois cenários de aplicações P2P precisam de mecanismos de incentivo e punição para evitar o comportamento de apenas consumir recursos sem contribuição de seus usuários.

## Objetivo

O objetivo do projeto é implementar um sistema de armazenamento e compartilhamento de dados distribuído com mecanismos de incentivo econômico para estimular a colaboração dos usuários e mecanismos de punição para evitar a prática de free riding ².
Como a natureza do projeto é distribuída, o seu sistema de pagamentos deve ser capaz de funcionar em ambiente de confiança zero entre os pares. Além disso, a consolidação destes pagamentos deve ocorrer com o mínimo de oneração de banda, processamento e memória, tanto por parte do árbitro do sistema quanto entre os pares, que estariam engajados, paralelamente, em um sessão de transmissão de arquivos.

## Arquitetura

O Torrente é a aplicação utilizada pelos usuários (peers) na rede swarm, para distribuição e armazenamento de dados. Ela é formada pelo protocolo BitTorrent que foi utilizado como base e modificado para atender os requisitos do projeto.

<p align="center">
  <img src="/Torrente/assets/torrente_flow.jpeg" />
</p>

Para viabilizar as rotinas de pagamento seguro, o sistema conta com o Payfluxo, uma aplicação que se comunica com a infraestrutura baseada em Fabric Hyperledger SDK.
Cada organização da Federação possui um nó que compõe uma infraestrutura de rede distribuída, constituída de certificadores, ordenadores e validadores para uma mesma Blockchain que registra os blocos de transações realizados no sistema.
O sistema permite aos usuários a cifração dos conteúdos transmitidos de tal forma que estes possam comercializar as chaves de acesso separadamente em um marketplace conforme a conveniência deste caso de uso para cada usuário.
Os usuários adquirem pedaços de arquivos incrementalmente pela rede efetuando micro pagamentos diretamente aos seus pares. Estes fundos então podem ser convertidos e consolidados frente ao árbitro financeiro do sistema, que pode ser tanto um Smart Contract, quanto um provedor de pagamentos tradicional.

<p align="center">
  <img src="/Torrente/assets/architecture.jpeg" />
</p>

## Video de Apresentação

<style>.embed-container { position: relative; padding-bottom: 56.25%; height: 0; overflow: hidden; max-width: 100%; } .embed-container iframe, .embed-container object, .embed-container embed { position: absolute; top: 0; left: 0; width: 100%; height: 100%; }</style><div class='embed-container'><iframe src='https://www.youtube.com/embed/QCz42cDgn_U' frameborder='0' allowfullscreen></iframe></div>

## Conclusão

O Torrente se apresenta como uma solução interessante para contextos distribuídos optando tanto por meios de pagamento tradicionais no meio digital, quanto por meios de pagamento baseados em um livro razão público de Blockhain. 
Ao atender os requisitos de segurança específicos do contexto motivador do Biobank, este projeto também sugere viabilidade de aplicação em outros contextos que, semelhantemente, demandem disponibilidade, integridade e confidencialidade na transmissão de dados, bem como um sistema de pagamentos dotado de autenticidade e não repúdio em ambiente de confiança zero entre os pares.

## Orientadores e Integrantes

**Orientadores:**

- Prof. Dr. Marcos Antônio Simplício Júnior
- Leonardo Toshinobu Kimura

**Alunos:**

- Felipe Kenzo Shiraishi
- Hector Kobayashi Yassuda
- Vitor Hugo Perles

## Referências

1. KIMURA, Leonardo, SILVA, Gustavo, KOGACHI, Lucas. Amazon Biobank:
Blockchain para processamento de DNA. 2020. Escola
Politécnica de Engenharia, Universidade de São Paulo, São Paulo, 2020. Disponível
em: <https://pcs.usp.br/pcspf/projetos-anteriores/2020-2/>. Acesso em: 03/02/2021.
2. HANG, Kan, ANTONOPOULOS, Nick, MAHMOOD, Zaigham. A Review of Incentive Mechanisms in Peer-to-Peer Systems. School of Computing, University
of Derby, Derby, UK, 2009.






