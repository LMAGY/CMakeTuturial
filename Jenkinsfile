Jenkinsfile (Declarative Pipeline)
pipeline {
    agent any
    stages {
        stage('Checkout') {
            steps {
                git branch: 'main',
                    url: 'https://github.com/LMAGY/MoreThanCoding.git'
            }
        }
        stage('Build') {
            steps {
                dir('build') {
                    sh 'cmake ../Memory_Pool'
                    sh 'make'
                }
            }
        }
        stage('Test') {
            steps {
                dir('build') {
                    sh './Memory_Pool'
                }
            }
        }
    }
}
